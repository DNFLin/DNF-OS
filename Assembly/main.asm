[BITS 16]           ; نبدأ في وضع 16-bit (Real Mode)
[ORG 0x7C00]        ; عنوان تحميل محمّل الإقلاع في الذاكرة

; ========== تعريف جدول الوصف العالمي (GDT) ==========
gdt_start:
    ; Descriptor #0: NULL descriptor
    dq 0x0, 0x0
    ; Descriptor #1: Code segment (4GB limit, 32-bit, execute/read)
    dq 0x0, 0x9A0000000000
    ; Descriptor #2: Data segment (4GB limit, 32-bit, read/write)
    dq 0x0, 0x930000000000

gdt_end:

; ========== الانتقال إلى Protected Mode ==========
start:
    cli                     ; تعطيل المقاطعات
    lgdt [gdt_start]         ; تحميل جدول GDT
    mov eax, cr0             ; تحميل قيمة CR0 (سجل التحكم)
    or eax, 1                ; تمكين الـ Protected Mode (ضبط البت 0)
    mov cr0, eax             ; حفظ التغييرات في CR0
    ; القفز إلى 32-bit Protected Mode
    jmp 08h:protected_mode   ; Far jump إلى الـ Code Segment في Protected Mode

; ========== الانتقال إلى وضع 32-bit (Protected Mode) ==========
[BITS 32]
protected_mode:
    ; تأكد من أننا في الـ Protected Mode
    ; تحميل سجل data segment (يجب تعيينه إلى الرقم 10h وهو descriptor رقم 2 في GDT)
    mov ax, 10h
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; الآن نحن في Protected Mode!
    ; طباعة رسالة للتأكد
    mov si, message
    call print_string

    jmp $                    ; حلقة لا نهائية لمنع إعادة التشغيل

; ========== دالة طباعة النص ==========
print_string:
    mov ah, 0x0E            ; وضع طباعة النص في وضع النص
.loop:
    lodsb                   ; تحميل الحرف التالي من [SI] إلى AL
    or al, al               ; تحقق مما إذا كان الحرف هو NULL (نهاية النص)
    jz done
    int 0x10                ; مقاطعة الفيديو لطباعة الحرف
    jmp .loop
done:
    ret

; ========== البيانات ==========
message db "Now in Protected Mode!", 0

; ========== نهاية محمّل الإقلاع ==========
times 510 - ($ - $$) db 0   ; ملء بقية الـ 512 بايت بالأصفار
dw 0xAA55                   ; التوقيع السحري لمحمّل الإقلاع
