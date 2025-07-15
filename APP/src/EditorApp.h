#pragma once
#include <ARCAD.h>
#include <ARMAT.h>

class ExampleLayer : public ar::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{ 
		auto va = ar::VertexArray::Create();
		vao = std::shared_ptr<ar::VertexArray>(va);

		std::string vertexSrc = R"(
			#version 450 core
			layout (location = 0) in vec3 a_Position;   
			layout (location = 1) in vec3 a_Color;
  
			out vec3 color;

			void main()
			{
				gl_Position = vec4(a_Position, 1.0);
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
		vao->Bind();
		glUseProgram(program);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	void OnEvent(ar::Event& event) override
	{
		if (event.GetEventType() == ar::EventType::KeyPressed)
		{
			AR_TRACE(event);
		}
	}

	void OnImGuiRender() override
	{
		
	}
private:
	unsigned int vShader, pShader, program;
	std::shared_ptr<ar::VertexBuffer> vbo;
	std::shared_ptr<ar::VertexArray> vao;
};

class EditorApp : public ar::Application
{
public:
	EditorApp();
	~EditorApp();
};
