#include "../Memory/DMA.h"

void SatLib::DMA_IRQHandler()
{
	switch(__even_in_range((DMAIV), DMAIV__DMA7IFG))
	{
	case DMAIV__DMA0IFG:
		xSemaphoreGiveFromISR(SatLib::DMA_Channels[0].finished, &SatLib::xHigherPriorityTaskWokenByDMA);
		break;
	case DMAIV__DMA1IFG:
		xSemaphoreGiveFromISR(SatLib::DMA_Channels[1].finished, &SatLib::xHigherPriorityTaskWokenByDMA);
		break;
	case DMAIV__DMA2IFG:
		xSemaphoreGiveFromISR(SatLib::DMA_Channels[2].finished, &SatLib::xHigherPriorityTaskWokenByDMA);
		break;
	case DMAIV__DMA3IFG:
		xSemaphoreGiveFromISR(SatLib::DMA_Channels[3].finished, &SatLib::xHigherPriorityTaskWokenByDMA);
		break;
	case DMAIV__DMA4IFG:
		xSemaphoreGiveFromISR(SatLib::DMA_Channels[4].finished, &SatLib::xHigherPriorityTaskWokenByDMA);
		break;
	case DMAIV__DMA5IFG:
		xSemaphoreGiveFromISR(SatLib::DMA_Channels[5].finished, &SatLib::xHigherPriorityTaskWokenByDMA);
		break;
	case DMAIV__DMA6IFG:
		xSemaphoreGiveFromISR(SatLib::DMA_Channels[6].finished, &SatLib::xHigherPriorityTaskWokenByDMA);
		break;
	case DMAIV__DMA7IFG:
		xSemaphoreGiveFromISR(SatLib::DMA_Channels[7].finished, &SatLib::xHigherPriorityTaskWokenByDMA);
		break;
	case DMAIV_NONE:
		__no_operation(); // will probably never run, but put this here for the debugger in case it does.
		break;
	default:
		__no_operation();
		break;
	}
	portYIELD_FROM_ISR(SatLib::xHigherPriorityTaskWokenByDMA);
}
