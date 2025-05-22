
#ifndef DESKX_DISPLAY_D3D11_HPP
#define DESKX_DISPLAY_D3D11_HPP

#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>

namespace display {

class d3d11 : public tpl {
private:
	IDXGIOutputDuplication *outdup = nullptr;
	ID3D11DeviceContext *ctx = nullptr;
	ID3D11Device *device = nullptr;
	IDXGIResource *deskres = nullptr;
    ID3D11Texture2D *img0, *img1 = nullptr;
	DXGI_OUTDUPL_DESC outdesc;
	D3D11_TEXTURE2D_DESC desc;
	DXGI_OUTDUPL_FRAME_INFO fi;
	D3D11_MAPPED_SUBRESOURCE sres;

public:
	bool
	init(void);

	void
	close(void);

	xy
	res(void);

	void
	refresh(pixs &);

	void
	set(const events &);
};

}

#endif