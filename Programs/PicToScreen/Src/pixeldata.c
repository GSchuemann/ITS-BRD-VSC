#include "pixeldata.h"
#include "errorhandler.h"
#include "input.h"
#include "palette.h"
#include <stdio.h>

int pixeldata_skip_gap(const BITMAPFILEHEADER *file_hdr_p,
                       const BITMAPINFOHEADER *info_hdr_p) {
   DWORD header_bytes = (DWORD)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
   DWORD palette_bytes = (DWORD)palette_entry_count(info_hdr_p) * (DWORD)sizeof(RGBQUAD);
   DWORD bytes_read = header_bytes + palette_bytes;

   CHECK_OR_RETURN(file_hdr_p->bfOffBits >= bytes_read, FORMAT_ERR);
   for (DWORD i = bytes_read; i < file_hdr_p->bfOffBits; i++) {
      CHECK_OR_RETURN(EOF != nxt_chr(), READ_ERR);
   }
   
   return EOK;
}