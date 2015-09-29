#include <windows.h>

bool screenCapturePart(TCHAR* fname){
    HDC hdcSource = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcSource);

    int capX = GetDeviceCaps(hdcSource, HORZRES);
    int capY = GetDeviceCaps(hdcSource, VERTRES);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdcSource, capX, capY);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, hBitmap);

    BitBlt(hdcMemory, 0, 0, capX, capY, hdcSource, 0, 0, SRCCOPY);
    hBitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

    DeleteDC(hdcSource);
    DeleteDC(hdcMemory);

    HPALETTE hpal = NULL;
    if (saveBitmap(fname, hBitmap, hpal)) return true;
    return false;
}

void screenCapture(HBITMAP& bitmap){
    HDC hdcSource = GetDC(NULL);
    HDC hdcMemory = CreateCompatibleDC(hdcSource);

    int capX = GetDeviceCaps(hdcSource, HORZRES);
    int capY = GetDeviceCaps(hdcSource, VERTRES);

    bitmap = CreateCompatibleBitmap(hdcSource, capX, capY);
    HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMemory, bitmap);

    BitBlt(hdcMemory, 0, 0, capX, capY, hdcSource, 0, 0, SRCCOPY);
    bitmap = (HBITMAP)SelectObject(hdcMemory, hBitmapOld);

    DeleteDC(hdcSource);
    DeleteDC(hdcMemory);
}

void Compare(
        HBITMAP Bitmap1,   // The first bitmap
        HBITMAP Bitmap2,   // The second bitmap
        COLORREF ClrDiff)              // Colour used for pixels that are different
{
    HDC hdcSource = GetDC(NULL);
    int W = GetDeviceCaps(hdcSource, HORZRES);
    int H = GetDeviceCaps(hdcSource, VERTRES);

    // Create the bitmaps needed
    HBITMAP FinalBitmap = CreateCompatibleBitmap(hdcSource, W, H);

    // DCs for all the bitmaps
    HDC Bitmap1DC = CreateCompatibleDC(hdcSource);
    HDC Bitmap2DC = CreateCompatibleDC(hdcSource);
    HDC FinalBitmapDC = CreateCompatibleDC(hdcSource);

    // Select the bitmaps into the DCs
    SelectObject(Bitmap1DC, Bitmap1);
    SelectObject(Bitmap2DC, Bitmap2);
    SelectObject(FinalBitmapDC, FinalBitmap);
    for (int w = 0; w < W; ++w)
    {
        for (int h = 0; h < H; ++h)
        {
            const auto pixel = GetPixel(Bitmap1DC, w, h);
            if (pixel != GetPixel(Bitmap2DC, w, h))
                SetPixel(FinalBitmapDC, w, h, RGB(0, 255, 0));
            else
                SetPixel(FinalBitmapDC, w, h, pixel);
        }
    }
    HPALETTE hpal = NULL;
    if (saveBitmap(L"d:\\merge.bmp", FinalBitmap, hpal));// return true;
    DeleteDC(hdcSource);
    DeleteDC(Bitmap1DC);
    DeleteDC(Bitmap2DC);
}
