#include "Vis2Application.h"
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/point_types.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <Eigen/Eigenvalues>
#include <glm/gtc/matrix_transform.hpp>

Vis2Application::Vis2Application(int width, int height, int ctx_major, int ctx_minor) : GLFWApplication(width, height, ctx_major, ctx_minor), quad()
{
	
}

void Vis2Application::init()
{
  // camera

  view = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));

  using Point3D = pcl::PointXYZRGBNormal; // defines the used point type

  // load pointcloud
  pcl::PointCloud<Point3D>::Ptr cloud(new pcl::PointCloud<Point3D>);
  pcl::PCDReader reader;
  reader.read<Point3D>("../data/bunny.pcd", *cloud);
  numPoints = cloud->size();
  std::cerr << "Read cloud: " << std::endl;
  std::cerr << *cloud << std::endl;

  pcl::KdTreeFLANN<Point3D> kdtree;
  kdtree.setInputCloud(cloud);

  struct Vertex
  {
	  glm::vec3 pos;
	  glm::vec3 normal;
	  glm::vec3 color;
	  glm::vec4 splatAxis;
  };

  std::vector<Vertex> model;
  model.reserve(cloud->size());

  const int knn = 8;
  for (size_t i = 0; i < cloud->size(); ++i)
  {
	  Point3D& searchPoint = cloud->points[i];
	  std::vector<int> pointIdxNKNSearch(knn);
	  std::vector<float> pointNKNSquaredDistance(knn);
	  kdtree.nearestKSearch(searchPoint, knn, pointIdxNKNSearch, pointNKNSquaredDistance);

	  std::vector<double> data;
	  for (int n = 0; n < pointIdxNKNSearch.size(); ++n)
	  {
	    int idx = pointIdxNKNSearch[n];
	    const Point3D& p = cloud->points[idx];
	    data.push_back(p.x); data.push_back(p.y); data.push_back(p.z);
	  }
	  ////// eigenvector computation with libeigen /////////
	  Eigen::Map<Eigen::MatrixXd> mat = Eigen::Map<Eigen::MatrixXd>(data.data(), 3, data.size() / 3);
	  Eigen::MatrixXd centered = mat.transpose().rowwise() - mat.transpose().colwise().mean();
	  Eigen::MatrixXd cov = centered.adjoint() * centered;
	  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov);

	  Eigen::MatrixXd eigvec = eig.eigenvectors();
	  Eigen::Vector3d eigVec0 = Eigen::Vector3d(eigvec(0, 0), eigvec(1, 0), eigvec(2, 0));// / eig.eigenvalues()[0];
	  Eigen::Vector3d eigVec2 = Eigen::Vector3d(eigvec(0, 2), eigvec(1, 2), eigvec(2, 2));// / eig.eigenvalues()[2];

	  float majorLength = sqrt(eig.eigenvalues()[2]) / (pointIdxNKNSearch.size() - 1);
	  float minorLength = sqrt(eig.eigenvalues()[1]) / (pointIdxNKNSearch.size() - 1);
	  float lengthFactor = minorLength / majorLength;

	  const Point3D& p = searchPoint;
	  Vertex v;
	  v.pos = glm::vec3(p.x, p.y, p.z);
	  v.normal = glm::vec3(eigVec0.x(), eigVec0.y(), eigVec0.z()); // Eigenvector with smallest eigenvalue is normal
	  v.color = glm::vec3(p.r, p.g, p.b);
	  v.splatAxis = glm::vec4(majorLength * eigVec2.x(), majorLength * eigVec2.y(), majorLength * eigVec2.z(), lengthFactor); // Eigenvector with highest eigenvalue is major axis of the ellipse, lengthFactor is the scaling to the minor axis
	  model.push_back(v);
  }

  // http://www.pcl-users.org/Passing-point-color-data-to-opengl-td3047230.html
  glGenBuffers(1, &vbPointBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vbPointBuffer);
  glBufferData(GL_ARRAY_BUFFER, model.size()*sizeof(Vertex), &model[0], GL_STATIC_DRAW);
  glGenVertexArrays(1, &va);
  glBindVertexArray(va);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, true, sizeof(Vertex), (void*)(3 * sizeof(float)));
  glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(6 * sizeof(float)));
  glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(Vertex), (void*)(9 * sizeof(float)));
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // fullscreen quad
  quad.init();

  // textures
  glGenTextures(1, &tex.depth);
  glGenTextures(1, &tex.accumWeight);
  glGenTextures(1, &tex.accumNormal);
  glGenTextures(1, &tex.accumColor);
  resize(getWidth(), getHeight()); // init textures 

  // render targets
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.accumWeight, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex.accumNormal, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tex.accumColor, 0);

  GLuint fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(fboStatus != GL_FRAMEBUFFER_COMPLETE)
	  std::cerr << "Error creating fbo (" << fboStatus << ")" << " @" << __FILE__ << ":" << __LINE__ << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glGenFramebuffers(1, &depthFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex.depth, 0);

  fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(fboStatus != GL_FRAMEBUFFER_COMPLETE)
	  std::cerr << "Error creating depth fbo (" << fboStatus << ")" << " @" << __FILE__ << ":" << __LINE__ << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClearColor(0, 0, 0, 1);

  const std::string shaderBaseDir = "../shaders/";

  GL::GLShader splatDepthVS(shaderBaseDir + "depth.vs", GL::VERTEX_SHADER);
  GL::GLShader splatGS(shaderBaseDir + "splat.gs", GL::GEOMETRY_SHADER);
  GL::GLShader splatDepthFS(shaderBaseDir + "depth.fs", GL::FRAGMENT_SHADER);
  GL::GLShader splatAttribVS(shaderBaseDir + "attrib.vs", GL::VERTEX_SHADER);
  GL::GLShader splatAttribFS(shaderBaseDir + "attrib.fs", GL::FRAGMENT_SHADER);
  GL::GLShader splatShadingVS(shaderBaseDir + "shading.vs", GL::VERTEX_SHADER);
  GL::GLShader splatShadingFS(shaderBaseDir + "shading.fs", GL::FRAGMENT_SHADER);

  splatDepthProg.initialize();
  splatDepthProg.attachShader(splatDepthVS);
  splatDepthProg.attachShader(splatGS);
  splatDepthProg.attachShader(splatDepthFS);
  splatDepthProg.link();

  splatAttribProg.initialize();
  splatAttribProg.attachShader(splatAttribVS);
  splatAttribProg.attachShader(splatGS);
  splatAttribProg.attachShader(splatAttribFS);
  splatAttribProg.link();

  splatShadingProg.initialize();
  splatShadingProg.attachShader(splatShadingVS);
  splatShadingProg.attachShader(splatShadingFS);
  splatShadingProg.link();

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);

  //glDisable(GL_CULL_FACE);
  //glDisable(GL_DEPTH_TEST);

  settings.splatSizeFactor = 1.5f;
}

void Vis2Application::update()
{
}

void Vis2Application::render()
{
  glm::mat4 mview = glm::lookAt(glm::vec3(0, 0.15, 0.15), glm::vec3(0, 0.1f, 0), glm::vec3(0, 1, 0));
  glm::mat4 mmodel = glm::rotate(glm::mat4(1), fmodf(getAbsTime() / 12.f, 2.f * glm::pi<float>()), glm::vec3(0, 1, 0));
  glm::vec4 lightPos = glm::vec4(1, 100, 1, 1);
  glm::vec3 lightColor = glm::vec3(1,1,1);

  //// Depth pass
  //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  //glEnable(GL_POLYGON_OFFSET_FILL);
  //glPolygonOffset(1.0f, 5.0f);

  splatDepthProg.bind();
  splatDepthProg.setUniform("projectionMatrix", projection);
  splatDepthProg.setUniform("viewMatrix", mview * mmodel);
  splatDepthProg.setUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mview * mmodel))));
  splatDepthProg.setUniform("radiusFactor", settings.splatSizeFactor);
  splatDepthProg.setUniform("useLength", settings.useLenght);
  glBindVertexArray(va);
  glDrawArrays(GL_POINTS, 0, numPoints);
  glBindVertexArray(0);
  

  ////  Attrib pass
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  const GLenum attPassBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attPassBuffers); // All Buffers!

  glClear(GL_COLOR_BUFFER_BIT); // clear only color buffer and use precomputed depths for depth testing
  //glDisable(GL_POLYGON_OFFSET_FILL);
  //glPolygonOffset(0.0f, 0.0f);

  // setting additive blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);

  splatAttribProg.bind();
  splatAttribProg.bindFragDataLocation(0, "outWeight");
  splatAttribProg.bindFragDataLocation(1, "outNormal");
  splatAttribProg.bindFragDataLocation(2, "outColor");
  splatAttribProg.setUniform("projectionMatrix", projection);
  splatAttribProg.setUniform("viewMatrix", mview * mmodel);
  splatAttribProg.setUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mview * mmodel))));
  splatAttribProg.setUniform("radiusFactor", settings.splatSizeFactor);
  splatAttribProg.setUniform("filterRadius", settings.filterRadius);
  splatAttribProg.setUniform("useLength", settings.useLenght);

  int loc = splatAttribProg.getUniformByName("texDepth");
  glUniform1i(loc, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex.depth);

  glDisable(GL_DEPTH_TEST);
  glBindVertexArray(va);
  glDrawArrays(GL_POINTS, 0, numPoints);
  glBindVertexArray(0);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

//  return;

  //// Shading pass
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  splatShadingProg.bind();
  splatShadingProg.setUniform("lightPos", glm::vec3(1));
  splatShadingProg.setUniform("lightColor", glm::vec3(1));
  splatShadingProg.setUniform("viewWorldTrans", glm::inverse(projection*mview));
  splatShadingProg.setUniform("camPos", glm::vec3(glm::inverse(mview)*glm::vec4(0, 0, 0, 1)));
  
  loc = splatShadingProg.getUniformByName("gb_color");
  glUniform1i(loc, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex.accumColor);

  loc = splatShadingProg.getUniformByName("gb_normal");
  glUniform1i(loc, 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, tex.accumNormal);

  loc = splatShadingProg.getUniformByName("gb_weight");
  glUniform1i(loc, 2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, tex.accumWeight);

  loc = splatShadingProg.getUniformByName("gb_depth");
  glUniform1i(loc, 3);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, tex.depth);

  quad.draw();
}

void Vis2Application::resize(int width, int height)
{
  glViewport(0, 0, width, height);

  glBindTexture(GL_TEXTURE_2D, tex.depth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

  glBindTexture(GL_TEXTURE_2D, tex.accumWeight);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, tex.accumNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, tex.accumColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, 0);

  projection = glm::perspective(20.f, (float)width / (float)height, 0.01f, 1.f);
}