#pragma once
#include <ARCAD.h>
#include <ARMAT.h>

class ExampleLayer : public ar::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{ 
		camera = std::make_shared<ar::PerspectiveCamera>(45.0f, 1920.0f / 1080.0f, 0.1f,
			100.0f, 5.0f);

		shader = std::shared_ptr<ar::Shader>(ar::Shader::Create(
			"resources/shaders/OpenGL/test.vert",
			"resources/shaders/OpenGL/test.frag"));

		auto va = ar::VertexArray::Create();
		vao = std::shared_ptr<ar::VertexArray>(va);

		std::string vertexSrc = R"(
			#version 450 core
			layout (location = 0) in vec3 a_Position;   
			layout (location = 1) in vec3 a_Color;

			uniform mat4 u_VP;
  
			out vec3 color;

			void main()
			{
				gl_Position = u_VP * vec4(a_Position, 1.0);
				color = a_Color; 
			}     
		)";

		std::string fragSrc = R"(
			#version 450 core
			out vec4 FragColor;  
			in vec3 color;
  
			void main()
			{
				FragColor = vec4(color, 1.0);
			}
		)";

		const char* vSrc = vertexSrc.c_str(), * pSrc = fragSrc.c_str();
		
		vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vSrc, nullptr);
		glCompileShader(vShader);

		pShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(pShader, 1, &pSrc, nullptr);
		glCompileShader(pShader);

		program = glCreateProgram();
		glAttachShader(program, vShader);
		glAttachShader(program, pShader);
		glLinkProgram(program);

		glDeleteShader(vShader);
		glDeleteShader(pShader);


		std::vector<ar::VertexPositionColor> verts = { 
			// position				color
			{ {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} },
			{ {0.5f, -0.5f, 0.0f} , {0.0f, 1.0f, 0.0f} },
			{ {0.0f,  0.5f, 0.0f} , {0.0f, 0.0f, 1.0f} }
		};
		auto vb = ar::VertexBuffer::Create(verts);
		vbo = std::shared_ptr<ar::VertexBuffer>(vb);
		va->AddVertexBuffer(vbo);
	}

	void OnUpdate() override
	{
		glUseProgram(program);
		auto location = glGetUniformLocation(program, "u_VP");
		glUniformMatrix4fv(location, 1, GL_FALSE, camera->GetVP().data.data());

		ar::RenderCommand::SetClearColor(ar::mat::Vec4(0.5f, 0.5f, 1.0f, 1.0f));
		ar::RenderCommand::Clear();
		
		ar::Renderer::BeginScene();

		ar::Renderer::Submit(vao);
		
		ar::Renderer::EndScene();
	}


	void OnImGuiRender() override
	{
		
	}

	void OnEvent(ar::Event& event) override;
	bool OnKeyPressed(ar::KeyPressedEvent& event);
	bool OnMouseMoved(ar::MouseMovedEvent& event);
	bool OnMouseScrolled(ar::MouseScrolledEvent& event);
	
private:
	unsigned int vShader, pShader, program;
	std::shared_ptr<ar::VertexBuffer> vbo;
	std::shared_ptr<ar::VertexArray> vao;
	std::shared_ptr<ar::PerspectiveCamera> camera;
	std::shared_ptr<ar::Shader> shader;
};

class EditorApp : public ar::Application
{
public:
	EditorApp();
	~EditorApp();
};
