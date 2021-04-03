#pragma once

#define CHWND_EXCEPT(hr) Window::HrException(__LINE__, __FILEW__, hr)
#define CHWND_LAST_EXCEPT() Window::HrException(__LINE__, __FILEW__, GetLastError())
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__, __FILEW__)
