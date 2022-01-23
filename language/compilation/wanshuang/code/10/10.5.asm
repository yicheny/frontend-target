assume cs:code

code segment
    start:  mov ax,6
            call ax
            inc ax 
        
            mov ax,4c00h
            int 21h
code ends
end start