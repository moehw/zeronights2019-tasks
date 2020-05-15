[bits 16]
org 0x7c00 ; offset in segment
start:
    call loader
    cli ; disable interrupts
    call setVESA
    
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov es, ax
    mov sp, 0xfffc
    
    ;sgdt[oldGDT]
    lgdt[descTable] ; load global descriptor table
    
    mov eax, cr0 
    or eax, 0x01 ; bit РЕ - protect enable
    mov cr0, eax
    
    ; to 32-bit mode
    ; cs changes to SEG_CODE
    jmp SEG_CODE:jump32
ret

loader:
    mov ah, 2
    mov al, 15
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, 0x80
    mov bx, 0x8000
    int 0x13 ; from 0x200 to 0x2000
    
    mov ah, 2
    mov al, 8
    mov ch, 0
    mov cl, 17
    mov dh, 0
    mov dl, 0x80
    mov bx, 0xA000
    int 0x13 ; from 0x2000 to 0x3000
    
ret

setVESA:
    mov ax, 0x4F01		; ah = 4F, function number
                        ; al = 00 - information about video-adapter, supported mode
                        ; 01 - verbose information
                        ; 02 - set mode
    mov cx, 0x118 		; video-mode 1024x768, 16777216 colors
    mov di, modebuf 
    int 0x10			; return ax = 0 - not supported, di - save information
    
                        ; 02 - set videomode
                        ; in extended video-adapter mode we can use output through frame-buffer or 64Kb-window
                        ; It achieved by setting bit in register bx while setting video-mode
    mov ax, 0x4f02
    mov bx, 0x118
    or bx, 0x4000 		; set bit, window
    int 0x10
    mov eax, dword[modebuf + 0x28] ; address of frame buffer
    mov dword[lfb], eax
ret 					; now we can use lfb

    [bits 32]
jump32:
    mov ax, SEG_DATA
    mov ss, ax
    mov es, ax
    mov ds, ax
    mov fs, ax
    mov gs, ax
    
    mov esp, 0xffff
    mov ebp, esp 

    mov esi, [lfb]
    push esi
    
    call SEG_CODE:0x8000
    
    jmp $ ; endless loop
ret

;------------------------------------------------------------------- - = DESC TABLE = -

startTable:
nullD: 			; zero descriptor
dd 0
dd 0

codeD: 			; code descriptor
dw 0xffff 		; limit, 64 Kb at this moment
dw 0			; base addr, 1-2 bytes
db 0 			; base addr, 3 byte
db 10011010b 	; flags 1
db 11001111b 	; flags 2
db 0			; base addr, 4 byte

dataD:
dw 0xffff
dw 0
db 0
db 10010010b
db 11001111b
db 0

code16D:
dw 0xffff 	
dw 0 		
db 0 		
db 10011010b 
db 0
db 0
endTable:

descTable:
dw endTable-startTable ; less 64Kb
dd startTable

SEG_DATA equ dataD-startTable
SEG_CODE equ codeD-startTable
SEG_16CODE equ code16D-startTable

;------------------------------------------------------------------- 

modebuf times 64 dd 0 ; 256 bytes - buffer, consist information about video-adapter
lfb dd 0 ; linear frame buffer