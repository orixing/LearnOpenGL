#include "SkyboxObj.h"
#include "Mesh.h"
#include "FrameBuffer.h"

SkyboxObj::SkyboxObj() : GameObj(nullptr) {
	vector<float> vertexDatas = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right      
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		// right face
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			// top face
		-1.0f, 1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right 
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};
	vector<unsigned int> indices = {
		0,1,2,
		1,0,3,
		4,5,6,
		6,7,4,
		8,9,10,
		10,11,8,
		12,13,14,
		13,12,15,
		16,17,18,
		18,19,16,
		20,21,22,
		21,20,23,
	};

	vector<Vertex> vertices = Vertex::genVertexByRawData(vertexDatas);
	//构造mesh
	Mesh* mesh = new Mesh(vertices, indices);
	//构造model
	GameObj::model = new Model(mesh);
	material = new SkyboxMaterial();
}
SkyboxObj::~SkyboxObj() {}


void SkyboxObj::PrecomputeIBL() {
	//todo:增加ShaderCtrl
	Shader HDRCubeShader("../../MyWindow/rawShaders/HDR2CubeVertexShader.vs", "../../MyWindow/rawShaders/HDR2CubeFragmentShader.fs");
	Shader diffuseCubeShader("../../MyWindow/rawShaders/diffuseCubeVertexShader.vs", "../../MyWindow/rawShaders/diffuseCubeFragmentShader.fs");
	Shader specularLightShader("../../MyWindow/rawShaders/diffuseCubeVertexShader.vs", "../../MyWindow/rawShaders/specularCubeFragmentShader.fs");
	Shader specularBRDFShader("../../MyWindow/rawShaders/screenVertexShader.vs", "../../MyWindow/rawShaders/specularBRDFFragmentShader.fs");

	//准备好一个cube的FBO和VAO
	FrameBuffer cubeFBO;

	cubeFBO.AddRenderbuffer(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, 1024, 1024);

	//创建一个cubemap纹理
	material->cubeMapCache = Texture::Builder().SetTarget(GL_TEXTURE_CUBE_MAP).SetInternalFormat(GL_RGB16F)
		.SetWidth(GlobalConst::CubeMapCacheSize).SetHeight(GlobalConst::CubeMapCacheSize).SetFormat(GL_RGB).SetMinFilter(GL_LINEAR).SetMagFilter(GL_LINEAR).Build();

	//将2DHDR图转换到cubeMap上，用六个相机分别绘制正方形，实际上就是拍下正方形的一个面
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	HDRCubeShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureCtrl::getInstance().getTexture(TextureEnum::SkyboxTex_AutumnPark)->id);
	HDRCubeShader.setInt("envHDRTex", 0);
	HDRCubeShader.setMat4("projection", captureProjection);
	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO.id);
	for (unsigned int i = 0; i < 6; ++i)
	{
		HDRCubeShader.setMat4("view", captureViews[i]);
		//cubeFBO.BindTexture(material->cubeMapCache, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, material->cubeMapCache->id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO.id);
		Draw();

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//准备diffuse天空盒
	//创建一个diffuseCube纹理
	material->IBLDiffuseLightTex = Texture::Builder().SetTarget(GL_TEXTURE_CUBE_MAP).SetInternalFormat(GL_RGB16F)
		.SetWidth(GlobalConst::diffuseCubeSize).SetHeight(GlobalConst::diffuseCubeSize).SetFormat(GL_RGB).SetMinFilter(GL_LINEAR).SetMagFilter(GL_LINEAR).Build();
	cubeFBO.AddRenderbuffer(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, 128, 128);


	diffuseCubeShader.use();
	diffuseCubeShader.setInt("environmentMap", 0);
	diffuseCubeShader.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, material->cubeMapCache->id);

	glViewport(0, 0, 128, 128); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, cubeFBO.id);
	for (unsigned int i = 0; i < 6; ++i)
	{
		diffuseCubeShader.setMat4("view", captureViews[i]);
		//cubeFBO.BindTexture(material->IBLDiffuseLightTex, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, material->IBLDiffuseLightTex->id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Draw();

	}
	//准备specular天空盒
	//创建一个specularCube纹理
	material->IBLSpecularLightTex = Texture::Builder().SetTarget(GL_TEXTURE_CUBE_MAP).SetInternalFormat(GL_RGB16F)
		.SetWidth(GlobalConst::specularCubeSize).SetHeight(GlobalConst::specularCubeSize).SetFormat(GL_RGB).SetMinFilter(GL_LINEAR_MIPMAP_LINEAR).SetMagFilter(GL_LINEAR).SetMipMap(true).Build();

	specularLightShader.use();
	specularLightShader.setInt("environmentMap", 0);
	specularLightShader.setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, material->cubeMapCache->id);

	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipSize = 1024 * std::pow(0.5, mip);

		cubeFBO.AddRenderbuffer(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, mipSize, mipSize);

		glViewport(0, 0, mipSize, mipSize);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		specularLightShader.setFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			specularLightShader.setMat4("view", captureViews[i]);

			cubeFBO.BindTexture(material->IBLSpecularLightTex, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Draw();
		}
	}

	//-----------
	float screenVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int screenVAO;
	glGenVertexArrays(1, &screenVAO);
	unsigned int screenVBO;
	glGenBuffers(1, &screenVBO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//------------准备specular BRDF
	material->IBLSpecularBRDFTex = Texture::Builder().SetTarget(GL_TEXTURE_2D).SetInternalFormat(GL_RG16F)
		.SetWidth(GlobalConst::specularBRDFTexSize).SetHeight(GlobalConst::specularBRDFTexSize).SetFormat(GL_RG).SetMinFilter(GL_LINEAR).SetMagFilter(GL_LINEAR).Build();


	cubeFBO.AddRenderbuffer(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GlobalConst::specularBRDFTexSize, GlobalConst::specularBRDFTexSize);
	cubeFBO.BindTexture(material->IBLSpecularBRDFTex, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);

	glViewport(0, 0, 512, 512);
	specularBRDFShader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(screenVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}