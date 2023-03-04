#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	VertexBuffer(const void* data, unsigned int size, const char* type);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};