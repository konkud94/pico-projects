#include <stdio.h>
#include <initializer_list>
#include <utility>
#include <array>
#include "pico/stdlib.h"
#include "pico/sync.h"
#include "hardware/spi.h"
#include "pinDefinitions/pinDefinitions.hpp"
#include "touchController/touchController.hpp"
#include "lcdController/lcdDriver.hpp"
#include "graphics/bitmap/bitmap12.hpp"
#include "graphics/utils/utils.hpp"
#include "graphics/bitmap/monochromaticBitmap.hpp"


/*
	important: max refresh rate is around 55 times per second!
*/
uint32_t GetTimeMSSinceBoot()
{
	const auto absoluteTime = get_absolute_time();
	const uint32_t msSinceBoot = to_ms_since_boot(absoluteTime);
	return msSinceBoot;
}


void FillBitmapWithTriangle(CMonochromaticBitmap& bitmap)
{
	bitmap.ClearAllPixels();
	const size_t bx = bitmap.GetDimensions().first;
	const size_t by = bitmap.GetDimensions().second;
	const float a = 2.0f * by / bx;
	const float b = (float)by;
	for(size_t y = 0; y < by; y++)
	{
		for(size_t x = 0; x < bx; x++)
		{
			const float y1 = -a * x + b;
			const float y2 = a * x - b;
			if(y >= y1 && y >= y2)
			{
				bitmap.SetPixelAt(x, y, true);
			}
		}
	}
}

int main() 
{
	stdio_init_all();
	spi_init(spi1, 4000000);
	//sleep_ms(1000 * 5);
	for(const auto pin : {CPinDefinitions::ChipSelectLcdPin,
		CPinDefinitions::ChipSelectTouchPadPin, CPinDefinitions::ChipSelectSDPin, CPinDefinitions::LcdBklPin,
		CPinDefinitions::LcdDcPin, CPinDefinitions::LcdRstPin})
	{
		gpio_init(pin);
		gpio_set_dir(pin, GPIO_OUT);
		gpio_put(pin, true);
	}
	for(const auto pin : {CPinDefinitions::SpiMosiPin,
		CPinDefinitions::SpiMisoPin, CPinDefinitions::SpiClkPin})
	{
		gpio_set_function(pin, GPIO_FUNC_SPI);
	}

	CLcdDriver* const lcdDriver = new CLcdDriver(spi1, CPinDefinitions::ChipSelectLcdPin,
		CPinDefinitions::LcdRstPin, CPinDefinitions::LcdDcPin);
	static constexpr size_t lcdBufferSize = lcdDriver->GetBitsPerPixel() * lcdDriver->GetPixelsAlongX() * lcdDriver->GetPixelsAlongY() / (size_t)8 ;
	uint8_t* const buffer = new uint8_t[lcdBufferSize];
	if(buffer == nullptr)
	{
		assert(false);
	}
	CColorBitmapInterface* const bitmap12 = new CBitmap12(240, 320, buffer);

	const char* red = "RED";
	const char* green = "GREEN";
	const char* blue = "BLUE";
	const char* black = "BLACK"; 
	const char* white = "WHITE";
	class CColorDescriptor {
	public:
		CColorDescriptor(const char* name, uint8_t red, uint8_t green, uint8_t blue)
		:	Name(name), Red(red), Green(green), Blue(blue)
		{
			;
		}
		const char* Name; 
		const uint8_t Red;
		const uint8_t Green;
		const uint8_t Blue;
	};
	std::array<CColorDescriptor, 5> colors = {
		CColorDescriptor(red, 255, 0, 0),
		CColorDescriptor(green, 0, 255, 0),
		CColorDescriptor(blue, 0, 0, 255),
		CColorDescriptor(black, 0, 0, 0),
		CColorDescriptor(white, 255, 255, 255),
	};


	{
		const auto& colorWhite = colors[4];
		const uint16_t pixel12BitWhite = CGraphicsUtils::RGBToPixel12Bit(colorWhite.Red, colorWhite.Green, colorWhite.Blue);
		const auto& colorGreen = colors[1];
		const uint16_t pixel12BitGreen = CGraphicsUtils::RGBToPixel12Bit(colorGreen.Red, colorGreen.Green, colorGreen.Blue);

		const size_t rectangleDiemnsionX = 60;
		const size_t rectangleDiemnsionY = 60;
		const size_t maxX = 239;
		const size_t maxY = 319;
		const size_t xyAdvanceRate = 1;
		size_t currentX = 0;
		size_t currentY = 0;
		const uint32_t sleepMs = 10;
		CMonochromaticBitmap* const triangleBitMap = new CMonochromaticBitmap(rectangleDiemnsionX, rectangleDiemnsionY, new uint8_t[450]);
		//triangleBitMap->SetAllPixels();
		FillBitmapWithTriangle(*triangleBitMap);
		while(true)
		{
			if(currentX <= maxX && currentY <= maxY)
			{
				bitmap12->SetWholeBufferToColor(pixel12BitWhite, CColorBitmapInterface::EPixelType::BitsPerPixel12);
				bitmap12->PutMonoBitmapAt(currentX, currentY, *triangleBitMap, pixel12BitGreen, pixel12BitWhite, CColorBitmapInterface::EPixelType::BitsPerPixel12);
				currentX += xyAdvanceRate;
				currentY += xyAdvanceRate;
				const uint32_t status = save_and_disable_interrupts();
				lcdDriver->FlushData(bitmap12->GetBuffer(), lcdBufferSize);
				restore_interrupts(status);
				sleep_ms(sleepMs);
			}
			else
			{
				currentX = 0;
				currentY = 0;
			}
		}
	}

	return 0;
}
