#include "Renderer.h"
#include "Walnut/Random.h"


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

void Renderer::Render()
{
	float aspeectRatio = m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();
	//Iterate through and render every pixel (THE MEAT AND POTATOES)
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec2 coord = { x / (float)m_FinalImage->GetWidth(), y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; //Maps coordinate -1 to 1
			coord.x *= aspeectRatio;
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDir(coord.x, coord.y, -1.0f);
	float radius = 0.5f;

	//Formula for finding the distance to an intersection of a sphere (centered on origin)
	//(bx^2+by^2+bz^2)t^2 + (2axbx + 2ayby + 2azbz)t + (ax^2 + ay^2 + az^2 - r^2) = 0
	//a = ray origin
	//b = ray direction
	//r = circle radius
	//t = dist to hit (from a)

	//Coefecients of our quadratic equation: ax^2 + bx + c = 0
	float a = glm::dot(rayDir, rayDir);
	float b = 2.0f * glm::dot(rayOrigin, rayDir);
	float c = glm::dot(rayOrigin, rayOrigin) - (radius * radius);

	//Quadratic Discriminant:
	// b^2 - 4ac
	float discriminant = (b * b) - 4.0f * (a * c);
	//Check for intersections
	//discrim > 0: 2 solutions
	//discrim = 0: 1 solution
	//discrim < 0: 0 solutions
	if (discriminant >= 0.0f)
	{
		//completing the Quadratic Formula:
		//(-b +- sqrt(dircriminant)) / (2.0f * a)
		float t0 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		float t1 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

		return 0xffff00ff;
	}

	return 0xffffffff;
}

