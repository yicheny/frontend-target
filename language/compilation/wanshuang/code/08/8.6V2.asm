mov ax,seg
mov ds,ax
mov bx,60h  ;确定记录地址，ds:bx

                            ;C: dec.pm = 38
mov word ptr [bx].0ch,38    ;排名字段改为38
                            ;C: dec.sr = dec.sr+70
add word ptr [bx].0eh,70    ;收入字段增加70

                                ;C: i=0
mov si,0                        ;使用si定位产品字符串中的字符
mov byte ptr [bx].10h[si],'V'   ;dec.cp[i] = 'V'
inc si                          ;i++
mov byte ptr [bx].10h[si],'A'   ;dec.cp[i] = 'A'
inc si                          ;i++
mov byte ptr [bx].10h[si],'X'   ;dec.cp[i] = 'X'
