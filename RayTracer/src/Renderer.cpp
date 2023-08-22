#include "Renderer.h"
#include "Walnut/Random.h"

void Renderer::Render()
{
	for (uint32_t i = 0; i < m_FinalImage->GetWidth() * m_FinalImage->GetHeight(); i++) {
		m_ImageData[i] = Walnut::Random::UInt();
		m_ImageData[i] |= 0xff000000;
	}

	m_FinalImage->SetData(m_ImageData);
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//No Resize Necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;
		//Resize
		m_FinalImage->Resize(width, height);
	}
	else
	{
		//Image Creation (of correct size)
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}
	//Regenerate new image data
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}
