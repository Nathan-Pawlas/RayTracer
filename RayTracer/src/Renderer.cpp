#include "Renderer.h"
#include "Walnut/Random.h"


namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		//Written on oneline for optimization: 
		// for each rgba channel it's multiplied by f (255) then converted to byte, giving a value between 0 & f
		// it is then bit shifted into the corresponding 32 bit channel (0x aa bb gg rr)
		return ((uint8_t)(color.a * 255.0f) << 24) | ((uint8_t)(color.b * 255.0f) << 16) | ((uint8_t)(color.g * 255.0f) << 8) | (uint8_t)(color.r * 255.0f);
	}
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

void Renderer::Render()
{
	float aspeectRatio = m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight();
	//Iterate through and render every pixel (THE MEAT AND POTATOES)
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			glm::vec2 coord = { x / (float)m_FinalImage->GetWidth(), y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; //Maps coordinate -1 to 1
			coord.x *= aspeectRatio;
			glm::vec4 color = PerPixel(coord);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}

	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
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
	if (discriminant < 0.0f)
		return glm::vec4(0,0,0,1); //No solutions, early return black

	//completing the Quadratic Formula:
	//(-b +- sqrt(dircriminant)) / (2.0f * a)
	float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
	float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
	//t1 < t0, a is squared so it can't be neg., thus because b is subtracted in t1, t1 must always be smaller than t0
	glm::vec3 hitPoint = rayOrigin + rayDir * t1;
	glm::vec3 normal = glm::normalize(hitPoint);
	glm::vec3 lightDir = glm::normalize(glm::vec3(1, 1, 1));

	float dotProduct = glm::max(glm::dot(normal, lightDir),0.0f); // = cos(angle)

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor *= dotProduct;
	return glm::vec4(sphereColor,1);
}

