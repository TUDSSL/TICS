; ModuleID = 'uart.bc'
target datalayout = "e-m:e-p:16:16-i32:16:32-a:16-n8:16"
target triple = "msp430"

%struct.uart_sm_t = type { i8, i8*, i16, i8, i8*, i16 }

@"\010x05C0" = external global i16, align 2
@"\010x05C6" = external global i8, align 1
@"\010x05C6+1" = external global i8, align 1
@"\010x05C8" = external global i16, align 2
@"\010x020A+1" = external global i8, align 1
@"\010x020C+1" = external global i8, align 1
@UART_SM = internal global %struct.uart_sm_t zeroinitializer, align 2
@"\010x05DA" = external global i16, align 2
@"\010x05DC" = external global i16, align 2
@"\010x05DE" = external global i16, align 2
@"\010x05CE" = external global i16, align 2
@"\010x05CC" = external constant i16, align 2
@"\010x05CA" = external global i8, align 1
@llvm.used = appending global [1 x i8*] [i8* bitcast (void ()* @USCI_A0_ISR to i8*)], section "llvm.metadata"

@__isr_24 = alias void (), void ()* @USCI_A0_ISR

; Function Attrs: nounwind
define void @UART_init() #0 {
entry:
  store volatile i16 1, i16* @"\010x05C0", align 2, !dbg !46
  %0 = load volatile i16, i16* @"\010x05C0", align 2, !dbg !47
  %or = or i16 %0, 128, !dbg !47
  store volatile i16 %or, i16* @"\010x05C0", align 2, !dbg !47
  store volatile i8 4, i8* @"\010x05C6", align 1, !dbg !48
  store volatile i8 0, i8* @"\010x05C6+1", align 1, !dbg !49
  store volatile i16 21825, i16* @"\010x05C8", align 2, !dbg !50
  %1 = load volatile i8, i8* @"\010x020A+1", align 1, !dbg !51
  %conv = zext i8 %1 to i16, !dbg !51
  %and = and i16 %conv, -2, !dbg !51
  %conv1 = trunc i16 %and to i8, !dbg !51
  store volatile i8 %conv1, i8* @"\010x020A+1", align 1, !dbg !51
  %2 = load volatile i8, i8* @"\010x020C+1", align 1, !dbg !52
  %conv2 = zext i8 %2 to i16, !dbg !52
  %or3 = or i16 %conv2, 1, !dbg !52
  %conv4 = trunc i16 %or3 to i8, !dbg !52
  store volatile i8 %conv4, i8* @"\010x020C+1", align 1, !dbg !52
  %3 = load volatile i8, i8* @"\010x020A+1", align 1, !dbg !53
  %conv5 = zext i8 %3 to i16, !dbg !53
  %and6 = and i16 %conv5, -3, !dbg !53
  %conv7 = trunc i16 %and6 to i8, !dbg !53
  store volatile i8 %conv7, i8* @"\010x020A+1", align 1, !dbg !53
  %4 = load volatile i8, i8* @"\010x020C+1", align 1, !dbg !54
  %conv8 = zext i8 %4 to i16, !dbg !54
  %or9 = or i16 %conv8, 2, !dbg !54
  %conv10 = trunc i16 %or9 to i8, !dbg !54
  store volatile i8 %conv10, i8* @"\010x020C+1", align 1, !dbg !54
  %5 = load volatile i16, i16* @"\010x05C0", align 2, !dbg !55
  %and11 = and i16 %5, -2, !dbg !55
  store volatile i16 %and11, i16* @"\010x05C0", align 2, !dbg !55
  store volatile i8 0, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 0), align 2, !dbg !56
  store volatile i16 0, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 2), align 2, !dbg !57
  store volatile i8 0, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !58
  store volatile i16 0, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 5), align 2, !dbg !59
  ret void, !dbg !60
}

; Function Attrs: nounwind
define void @UART_teardown() #0 {
entry:
  %0 = load volatile i8, i8* @"\010x020A+1", align 1, !dbg !61
  %conv = zext i8 %0 to i16, !dbg !61
  %and = and i16 %conv, -2, !dbg !61
  %conv1 = trunc i16 %and to i8, !dbg !61
  store volatile i8 %conv1, i8* @"\010x020A+1", align 1, !dbg !61
  %1 = load volatile i8, i8* @"\010x020C+1", align 1, !dbg !62
  %conv2 = zext i8 %1 to i16, !dbg !62
  %and3 = and i16 %conv2, -2, !dbg !62
  %conv4 = trunc i16 %and3 to i8, !dbg !62
  store volatile i8 %conv4, i8* @"\010x020C+1", align 1, !dbg !62
  %2 = load volatile i8, i8* @"\010x020A+1", align 1, !dbg !63
  %conv5 = zext i8 %2 to i16, !dbg !63
  %and6 = and i16 %conv5, -3, !dbg !63
  %conv7 = trunc i16 %and6 to i8, !dbg !63
  store volatile i8 %conv7, i8* @"\010x020A+1", align 1, !dbg !63
  %3 = load volatile i8, i8* @"\010x020C+1", align 1, !dbg !64
  %conv8 = zext i8 %3 to i16, !dbg !64
  %and9 = and i16 %conv8, -3, !dbg !64
  %conv10 = trunc i16 %and9 to i8, !dbg !64
  store volatile i8 %conv10, i8* @"\010x020C+1", align 1, !dbg !64
  store volatile i16 1, i16* @"\010x05C0", align 2, !dbg !65
  store volatile i8 0, i8* @"\010x05C6", align 1, !dbg !66
  store volatile i16 0, i16* @"\010x05C8", align 2, !dbg !67
  store volatile i16 0, i16* @"\010x05DA", align 2, !dbg !68
  store volatile i16 0, i16* @"\010x05DC", align 2, !dbg !69
  ret void, !dbg !70
}

; Function Attrs: nounwind
define void @UART_asyncSend(i8* %txBuf, i16 zeroext %size) #0 {
entry:
  %txBuf.addr = alloca i8*, align 2
  %size.addr = alloca i16, align 2
  store i8* %txBuf, i8** %txBuf.addr, align 2
  call void @llvm.dbg.declare(metadata i8** %txBuf.addr, metadata !71, metadata !72), !dbg !73
  store i16 %size, i16* %size.addr, align 2
  call void @llvm.dbg.declare(metadata i16* %size.addr, metadata !74, metadata !72), !dbg !75
  br label %while.cond, !dbg !76

while.cond:                                       ; preds = %while.body, %entry
  %0 = load volatile i8, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 0), align 2, !dbg !77
  %tobool = icmp ne i8 %0, 0, !dbg !76
  br i1 %tobool, label %while.body, label %while.end, !dbg !76

while.body:                                       ; preds = %while.cond
  br label %while.cond, !dbg !80

while.end:                                        ; preds = %while.cond
  store volatile i8 1, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 0), align 2, !dbg !82
  %1 = load i8*, i8** %txBuf.addr, align 2, !dbg !83
  store volatile i8* %1, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 1), align 2, !dbg !84
  %2 = load i16, i16* %size.addr, align 2, !dbg !85
  store volatile i16 %2, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 2), align 2, !dbg !86
  %3 = load volatile i16, i16* @"\010x05DC", align 2, !dbg !87
  %and = and i16 %3, -5, !dbg !87
  store volatile i16 %and, i16* @"\010x05DC", align 2, !dbg !87
  %4 = load volatile i16, i16* @"\010x05DA", align 2, !dbg !88
  %or = or i16 %4, 2, !dbg !88
  store volatile i16 %or, i16* @"\010x05DA", align 2, !dbg !88
  ret void, !dbg !89
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind
define void @UART_send(i8* %txBuf, i16 zeroext %size) #0 {
entry:
  %txBuf.addr = alloca i8*, align 2
  %size.addr = alloca i16, align 2
  store i8* %txBuf, i8** %txBuf.addr, align 2
  call void @llvm.dbg.declare(metadata i8** %txBuf.addr, metadata !90, metadata !72), !dbg !91
  store i16 %size, i16* %size.addr, align 2
  call void @llvm.dbg.declare(metadata i16* %size.addr, metadata !92, metadata !72), !dbg !93
  %0 = load i8*, i8** %txBuf.addr, align 2, !dbg !94
  %1 = load i16, i16* %size.addr, align 2, !dbg !95
  call void @UART_asyncSend(i8* %0, i16 zeroext %1), !dbg !96
  br label %while.cond, !dbg !97

while.cond:                                       ; preds = %while.body, %entry
  %2 = load volatile i8, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 0), align 2, !dbg !98
  %tobool = icmp ne i8 %2, 0, !dbg !97
  br i1 %tobool, label %while.body, label %while.end, !dbg !97

while.body:                                       ; preds = %while.cond
  br label %while.cond, !dbg !101

while.end:                                        ; preds = %while.cond
  ret void, !dbg !103
}

; Function Attrs: nounwind
define void @UART_critSend(i8* %txBuf, i16 zeroext %size) #0 {
entry:
  %txBuf.addr = alloca i8*, align 2
  %size.addr = alloca i16, align 2
  store i8* %txBuf, i8** %txBuf.addr, align 2
  call void @llvm.dbg.declare(metadata i8** %txBuf.addr, metadata !104, metadata !72), !dbg !105
  store i16 %size, i16* %size.addr, align 2
  call void @llvm.dbg.declare(metadata i16* %size.addr, metadata !106, metadata !72), !dbg !107
  br label %while.cond, !dbg !108

while.cond:                                       ; preds = %while.body, %entry
  %0 = load volatile i8, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 0), align 2, !dbg !109
  %tobool = icmp ne i8 %0, 0, !dbg !108
  br i1 %tobool, label %while.body, label %while.end, !dbg !108

while.body:                                       ; preds = %while.cond
  br label %while.cond, !dbg !112

while.end:                                        ; preds = %while.cond
  store volatile i8 1, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 0), align 2, !dbg !114
  %1 = load i8*, i8** %txBuf.addr, align 2, !dbg !115
  store volatile i8* %1, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 1), align 2, !dbg !116
  %2 = load i16, i16* %size.addr, align 2, !dbg !117
  store volatile i16 %2, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 2), align 2, !dbg !118
  %3 = load volatile i16, i16* @"\010x05DE", align 2, !dbg !119
  %and = and i16 %3, -5, !dbg !119
  store volatile i16 %and, i16* @"\010x05DE", align 2, !dbg !119
  br label %while.cond.1, !dbg !120

while.cond.1:                                     ; preds = %while.end.8, %while.end
  %4 = load volatile i16, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 2), align 2, !dbg !121
  %dec = add i16 %4, -1, !dbg !121
  store volatile i16 %dec, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 2), align 2, !dbg !121
  %tobool2 = icmp ne i16 %4, 0, !dbg !120
  br i1 %tobool2, label %while.body.3, label %while.end.10, !dbg !120

while.body.3:                                     ; preds = %while.cond.1
  %5 = load volatile i8*, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 1), align 2, !dbg !123
  %incdec.ptr = getelementptr inbounds i8, i8* %5, i32 1, !dbg !123
  store volatile i8* %incdec.ptr, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 1), align 2, !dbg !123
  %6 = load i8, i8* %5, align 1, !dbg !125
  %conv = zext i8 %6 to i16, !dbg !125
  store volatile i16 %conv, i16* @"\010x05CE", align 2, !dbg !126
  br label %while.cond.4, !dbg !127

while.cond.4:                                     ; preds = %while.body.7, %while.body.3
  %7 = load volatile i16, i16* @"\010x05DC", align 2, !dbg !128
  %and5 = and i16 %7, 2, !dbg !131
  %tobool6 = icmp ne i16 %and5, 0, !dbg !132
  %lnot = xor i1 %tobool6, true, !dbg !132
  br i1 %lnot, label %while.body.7, label %while.end.8, !dbg !127

while.body.7:                                     ; preds = %while.cond.4
  br label %while.cond.4, !dbg !133

while.end.8:                                      ; preds = %while.cond.4
  %8 = load volatile i16, i16* @"\010x05DC", align 2, !dbg !135
  %and9 = and i16 %8, -3, !dbg !135
  store volatile i16 %and9, i16* @"\010x05DC", align 2, !dbg !135
  br label %while.cond.1, !dbg !120

while.end.10:                                     ; preds = %while.cond.1
  store volatile i8 0, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 0), align 2, !dbg !136
  ret void, !dbg !137
}

; Function Attrs: nounwind
define zeroext i8 @UART_isTxBusy() #0 {
entry:
  %0 = load volatile i8, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 0), align 2, !dbg !138
  ret i8 %0, !dbg !139
}

; Function Attrs: nounwind
define void @UART_asyncReceive(i8* %rxBuf, i16 zeroext %size) #0 {
entry:
  %rxBuf.addr = alloca i8*, align 2
  %size.addr = alloca i16, align 2
  store i8* %rxBuf, i8** %rxBuf.addr, align 2
  call void @llvm.dbg.declare(metadata i8** %rxBuf.addr, metadata !140, metadata !72), !dbg !141
  store i16 %size, i16* %size.addr, align 2
  call void @llvm.dbg.declare(metadata i16* %size.addr, metadata !142, metadata !72), !dbg !143
  br label %while.cond, !dbg !144

while.cond:                                       ; preds = %while.body, %entry
  %0 = load volatile i8, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !145
  %tobool = icmp ne i8 %0, 0, !dbg !144
  br i1 %tobool, label %while.body, label %while.end, !dbg !144

while.body:                                       ; preds = %while.cond
  br label %while.cond, !dbg !148

while.end:                                        ; preds = %while.cond
  store volatile i8 1, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !150
  %1 = load i8*, i8** %rxBuf.addr, align 2, !dbg !151
  store volatile i8* %1, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 4), align 2, !dbg !152
  %2 = load i16, i16* %size.addr, align 2, !dbg !153
  store volatile i16 %2, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 5), align 2, !dbg !154
  %3 = load volatile i16, i16* @"\010x05DC", align 2, !dbg !155
  %and = and i16 %3, -2, !dbg !155
  store volatile i16 %and, i16* @"\010x05DC", align 2, !dbg !155
  %4 = load volatile i16, i16* @"\010x05DA", align 2, !dbg !156
  %or = or i16 %4, 1, !dbg !156
  store volatile i16 %or, i16* @"\010x05DA", align 2, !dbg !156
  ret void, !dbg !157
}

; Function Attrs: nounwind
define void @UART_receive(i8* %rxBuf, i16 zeroext %size) #0 {
entry:
  %rxBuf.addr = alloca i8*, align 2
  %size.addr = alloca i16, align 2
  store i8* %rxBuf, i8** %rxBuf.addr, align 2
  call void @llvm.dbg.declare(metadata i8** %rxBuf.addr, metadata !158, metadata !72), !dbg !159
  store i16 %size, i16* %size.addr, align 2
  call void @llvm.dbg.declare(metadata i16* %size.addr, metadata !160, metadata !72), !dbg !161
  %0 = load i8*, i8** %rxBuf.addr, align 2, !dbg !162
  %1 = load i16, i16* %size.addr, align 2, !dbg !163
  call void @UART_asyncReceive(i8* %0, i16 zeroext %1), !dbg !164
  br label %while.cond, !dbg !165

while.cond:                                       ; preds = %while.body, %entry
  %2 = load volatile i8, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !166
  %tobool = icmp ne i8 %2, 0, !dbg !165
  br i1 %tobool, label %while.body, label %while.end, !dbg !165

while.body:                                       ; preds = %while.cond
  br label %while.cond, !dbg !169

while.end:                                        ; preds = %while.cond
  ret void, !dbg !171
}

; Function Attrs: nounwind
define void @UART_critReceive(i8* %rxBuf, i16 zeroext %size) #0 {
entry:
  %rxBuf.addr = alloca i8*, align 2
  %size.addr = alloca i16, align 2
  %rec = alloca i8, align 1
  store i8* %rxBuf, i8** %rxBuf.addr, align 2
  call void @llvm.dbg.declare(metadata i8** %rxBuf.addr, metadata !172, metadata !72), !dbg !173
  store i16 %size, i16* %size.addr, align 2
  call void @llvm.dbg.declare(metadata i16* %size.addr, metadata !174, metadata !72), !dbg !175
  br label %while.cond, !dbg !176

while.cond:                                       ; preds = %while.body, %entry
  %0 = load volatile i8, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !177
  %tobool = icmp ne i8 %0, 0, !dbg !176
  br i1 %tobool, label %while.body, label %while.end, !dbg !176

while.body:                                       ; preds = %while.cond
  br label %while.cond, !dbg !180

while.end:                                        ; preds = %while.cond
  store volatile i8 1, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !182
  %1 = load i8*, i8** %rxBuf.addr, align 2, !dbg !183
  store volatile i8* %1, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 4), align 2, !dbg !184
  %2 = load i16, i16* %size.addr, align 2, !dbg !185
  store volatile i16 %2, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 5), align 2, !dbg !186
  %3 = load volatile i16, i16* @"\010x05DC", align 2, !dbg !187
  %and = and i16 %3, -2, !dbg !187
  store volatile i16 %and, i16* @"\010x05DC", align 2, !dbg !187
  br label %while.cond.1, !dbg !188

while.cond.1:                                     ; preds = %while.end.8, %while.end
  %4 = load volatile i16, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 5), align 2, !dbg !189
  %dec = add i16 %4, -1, !dbg !189
  store volatile i16 %dec, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 5), align 2, !dbg !189
  %tobool2 = icmp ne i16 %4, 0, !dbg !188
  br i1 %tobool2, label %while.body.3, label %while.end.10, !dbg !188

while.body.3:                                     ; preds = %while.cond.1
  br label %while.cond.4, !dbg !191

while.cond.4:                                     ; preds = %while.body.7, %while.body.3
  %5 = load volatile i16, i16* @"\010x05DC", align 2, !dbg !193
  %and5 = and i16 %5, 1, !dbg !196
  %tobool6 = icmp ne i16 %and5, 0, !dbg !197
  %lnot = xor i1 %tobool6, true, !dbg !197
  br i1 %lnot, label %while.body.7, label %while.end.8, !dbg !191

while.body.7:                                     ; preds = %while.cond.4
  br label %while.cond.4, !dbg !198

while.end.8:                                      ; preds = %while.cond.4
  %6 = load volatile i16, i16* @"\010x05DC", align 2, !dbg !200
  %and9 = and i16 %6, -2, !dbg !200
  store volatile i16 %and9, i16* @"\010x05DC", align 2, !dbg !200
  call void @llvm.dbg.declare(metadata i8* %rec, metadata !201, metadata !72), !dbg !202
  %7 = load volatile i16, i16* @"\010x05CC", align 2, !dbg !203
  %conv = trunc i16 %7 to i8, !dbg !203
  store i8 %conv, i8* %rec, align 1, !dbg !202
  %8 = load i8, i8* %rec, align 1, !dbg !204
  %9 = load volatile i8*, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 4), align 2, !dbg !205
  %incdec.ptr = getelementptr inbounds i8, i8* %9, i32 1, !dbg !205
  store volatile i8* %incdec.ptr, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 4), align 2, !dbg !205
  store i8 %8, i8* %9, align 1, !dbg !206
  br label %while.cond.1, !dbg !188

while.end.10:                                     ; preds = %while.cond.1
  store volatile i8 0, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !207
  ret void, !dbg !208
}

; Function Attrs: nounwind
define zeroext i8 @UART_isRxBusy() #0 {
entry:
  %0 = load volatile i8, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !209
  ret i8 %0, !dbg !210
}

; Function Attrs: nounwind
define zeroext i8 @UART_isRxDone() #0 {
entry:
  %0 = load volatile i8, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !211
  %tobool = icmp ne i8 %0, 0, !dbg !212
  %lnot = xor i1 %tobool, true, !dbg !212
  %lnot.ext = zext i1 %lnot to i16, !dbg !212
  %conv = trunc i16 %lnot.ext to i8, !dbg !212
  ret i8 %conv, !dbg !213
}

; Function Attrs: noinline nounwind
define msp430_intrcc void @USCI_A0_ISR() #2 {
entry:
  %rec = alloca i8, align 1
  call void @llvm.dbg.declare(metadata i8* %rec, metadata !214, metadata !72), !dbg !215
  %0 = load volatile i16, i16* @"\010x05DE", align 2, !dbg !216
  switch i16 %0, label %sw.epilog [
    i16 0, label %sw.bb
    i16 2, label %sw.bb.1
    i16 4, label %sw.bb.5
    i16 6, label %sw.bb.17
    i16 8, label %sw.bb.18
  ], !dbg !217

sw.bb:                                            ; preds = %entry
  br label %sw.epilog, !dbg !218

sw.bb.1:                                          ; preds = %entry
  %1 = load volatile i16, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 5), align 2, !dbg !220
  %dec = add i16 %1, -1, !dbg !220
  store volatile i16 %dec, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 5), align 2, !dbg !220
  %tobool = icmp ne i16 %1, 0, !dbg !220
  br i1 %tobool, label %if.then, label %if.end, !dbg !222

if.then:                                          ; preds = %sw.bb.1
  %2 = load volatile i16, i16* @"\010x05CC", align 2, !dbg !223
  %conv = trunc i16 %2 to i8, !dbg !223
  store i8 %conv, i8* %rec, align 1, !dbg !225
  %3 = load i8, i8* %rec, align 1, !dbg !226
  %4 = load volatile i8*, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 4), align 2, !dbg !227
  %incdec.ptr = getelementptr inbounds i8, i8* %4, i32 1, !dbg !227
  store volatile i8* %incdec.ptr, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 4), align 2, !dbg !227
  store i8 %3, i8* %4, align 1, !dbg !228
  br label %if.end, !dbg !229

if.end:                                           ; preds = %if.then, %sw.bb.1
  %5 = load volatile i16, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 5), align 2, !dbg !230
  %cmp = icmp eq i16 0, %5, !dbg !232
  br i1 %cmp, label %if.then.3, label %if.end.4, !dbg !233

if.then.3:                                        ; preds = %if.end
  %6 = load volatile i16, i16* @"\010x05DA", align 2, !dbg !234
  %and = and i16 %6, -2, !dbg !234
  store volatile i16 %and, i16* @"\010x05DA", align 2, !dbg !234
  store volatile i8 0, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 3), align 2, !dbg !236
  br label %if.end.4, !dbg !237

if.end.4:                                         ; preds = %if.then.3, %if.end
  br label %sw.epilog, !dbg !238

sw.bb.5:                                          ; preds = %entry
  %7 = load volatile i8*, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 1), align 2, !dbg !239
  %incdec.ptr6 = getelementptr inbounds i8, i8* %7, i32 1, !dbg !239
  store volatile i8* %incdec.ptr6, i8** getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 1), align 2, !dbg !239
  %8 = load i8, i8* %7, align 1, !dbg !240
  %conv7 = zext i8 %8 to i16, !dbg !240
  store volatile i16 %conv7, i16* @"\010x05CE", align 2, !dbg !241
  %9 = load volatile i16, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 2), align 2, !dbg !242
  %dec8 = add i16 %9, -1, !dbg !242
  store volatile i16 %dec8, i16* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 2), align 2, !dbg !242
  %cmp9 = icmp eq i16 %dec8, 0, !dbg !244
  br i1 %cmp9, label %if.then.11, label %if.end.16, !dbg !245

if.then.11:                                       ; preds = %sw.bb.5
  br label %while.cond, !dbg !246

while.cond:                                       ; preds = %while.body, %if.then.11
  %10 = load volatile i8, i8* @"\010x05CA", align 1, !dbg !248
  %conv12 = zext i8 %10 to i16, !dbg !248
  %and13 = and i16 %conv12, 1, !dbg !251
  %tobool14 = icmp ne i16 %and13, 0, !dbg !246
  br i1 %tobool14, label %while.body, label %while.end, !dbg !246

while.body:                                       ; preds = %while.cond
  br label %while.cond, !dbg !252

while.end:                                        ; preds = %while.cond
  %11 = load volatile i16, i16* @"\010x05DA", align 2, !dbg !254
  %and15 = and i16 %11, -3, !dbg !254
  store volatile i16 %and15, i16* @"\010x05DA", align 2, !dbg !254
  store volatile i8 0, i8* getelementptr inbounds (%struct.uart_sm_t, %struct.uart_sm_t* @UART_SM, i32 0, i32 0), align 2, !dbg !255
  br label %if.end.16, !dbg !256

if.end.16:                                        ; preds = %while.end, %sw.bb.5
  br label %sw.epilog, !dbg !257

sw.bb.17:                                         ; preds = %entry
  br label %sw.epilog, !dbg !258

sw.bb.18:                                         ; preds = %entry
  br label %sw.epilog, !dbg !259

sw.epilog:                                        ; preds = %entry, %sw.bb.18, %sw.bb.17, %if.end.16, %if.end.4, %sw.bb
  ret void, !dbg !260
}

attributes #0 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { noinline nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!43, !44}
!llvm.ident = !{!45}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 3.8.0 (http://llvm.org/git/clang.git 52ed5ec631b0bbf5c714baa0cd83c33ebfe0c6aa) (http://llvm.org/git/llvm.git 81386d4b4fdd80f038fd4ebddc59613770ea236c)", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3, globals: !31)
!1 = !DIFile(filename: "../../src/wired/uart.c", directory: "/home/reviewer/src/apps/app-temp-log-alpaca-new/ext/libwispbase/bld/clang")
!2 = !{}
!3 = !{!4, !7, !8, !20, !21, !22, !25, !26, !27, !28, !29, !30}
!4 = distinct !DISubprogram(name: "UART_init", scope: !1, file: !1, line: 32, type: !5, isLocal: false, isDefinition: true, scopeLine: 32, flags: DIFlagPrototyped, isOptimized: false, function: void ()* @UART_init, variables: !2)
!5 = !DISubroutineType(types: !6)
!6 = !{null}
!7 = distinct !DISubprogram(name: "UART_teardown", scope: !1, file: !1, line: 101, type: !5, isLocal: false, isDefinition: true, scopeLine: 102, flags: DIFlagPrototyped, isOptimized: false, function: void ()* @UART_teardown, variables: !2)
!8 = distinct !DISubprogram(name: "UART_asyncSend", scope: !1, file: !1, line: 124, type: !9, isLocal: false, isDefinition: true, scopeLine: 124, flags: DIFlagPrototyped, isOptimized: false, function: void (i8*, i16)* @UART_asyncSend, variables: !2)
!9 = !DISubroutineType(types: !10)
!10 = !{null, !11, !17}
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 16, align: 16)
!12 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint8_t", file: !13, line: 42, baseType: !14)
!13 = !DIFile(filename: "/opt/ti/mspgcc/msp430-elf/include/stdint.h", directory: "/home/reviewer/src/apps/app-temp-log-alpaca-new/ext/libwispbase/bld/clang")
!14 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint8_t", file: !15, line: 28, baseType: !16)
!15 = !DIFile(filename: "/opt/ti/mspgcc/msp430-elf/include/machine/_default_types.h", directory: "/home/reviewer/src/apps/app-temp-log-alpaca-new/ext/libwispbase/bld/clang")
!16 = !DIBasicType(name: "unsigned char", size: 8, align: 8, encoding: DW_ATE_unsigned_char)
!17 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint16_t", file: !13, line: 54, baseType: !18)
!18 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint16_t", file: !15, line: 38, baseType: !19)
!19 = !DIBasicType(name: "unsigned short", size: 16, align: 16, encoding: DW_ATE_unsigned)
!20 = distinct !DISubprogram(name: "UART_send", scope: !1, file: !1, line: 152, type: !9, isLocal: false, isDefinition: true, scopeLine: 152, flags: DIFlagPrototyped, isOptimized: false, function: void (i8*, i16)* @UART_send, variables: !2)
!21 = distinct !DISubprogram(name: "UART_critSend", scope: !1, file: !1, line: 165, type: !9, isLocal: false, isDefinition: true, scopeLine: 165, flags: DIFlagPrototyped, isOptimized: false, function: void (i8*, i16)* @UART_critSend, variables: !2)
!22 = distinct !DISubprogram(name: "UART_isTxBusy", scope: !1, file: !1, line: 191, type: !23, isLocal: false, isDefinition: true, scopeLine: 191, isOptimized: false, function: i8 ()* @UART_isTxBusy, variables: !2)
!23 = !DISubroutineType(types: !24)
!24 = !{!12}
!25 = distinct !DISubprogram(name: "UART_asyncReceive", scope: !1, file: !1, line: 201, type: !9, isLocal: false, isDefinition: true, scopeLine: 201, flags: DIFlagPrototyped, isOptimized: false, function: void (i8*, i16)* @UART_asyncReceive, variables: !2)
!26 = distinct !DISubprogram(name: "UART_receive", scope: !1, file: !1, line: 228, type: !9, isLocal: false, isDefinition: true, scopeLine: 228, flags: DIFlagPrototyped, isOptimized: false, function: void (i8*, i16)* @UART_receive, variables: !2)
!27 = distinct !DISubprogram(name: "UART_critReceive", scope: !1, file: !1, line: 241, type: !9, isLocal: false, isDefinition: true, scopeLine: 241, flags: DIFlagPrototyped, isOptimized: false, function: void (i8*, i16)* @UART_critReceive, variables: !2)
!28 = distinct !DISubprogram(name: "UART_isRxBusy", scope: !1, file: !1, line: 269, type: !23, isLocal: false, isDefinition: true, scopeLine: 269, isOptimized: false, function: i8 ()* @UART_isRxBusy, variables: !2)
!29 = distinct !DISubprogram(name: "UART_isRxDone", scope: !1, file: !1, line: 278, type: !23, isLocal: false, isDefinition: true, scopeLine: 278, isOptimized: false, function: i8 ()* @UART_isRxDone, variables: !2)
!30 = distinct !DISubprogram(name: "USCI_A0_ISR", scope: !1, file: !1, line: 290, type: !5, isLocal: false, isDefinition: true, scopeLine: 294, flags: DIFlagPrototyped, isOptimized: false, function: void ()* @USCI_A0_ISR, variables: !2)
!31 = !{!32}
!32 = !DIGlobalVariable(name: "UART_SM", scope: !0, file: !1, line: 25, type: !33, isLocal: true, isDefinition: true, variable: %struct.uart_sm_t* @UART_SM)
!33 = !DIDerivedType(tag: DW_TAG_volatile_type, baseType: !34)
!34 = !DIDerivedType(tag: DW_TAG_typedef, name: "uart_sm_t", file: !1, line: 23, baseType: !35)
!35 = !DICompositeType(tag: DW_TAG_structure_type, file: !1, line: 15, size: 96, align: 16, elements: !36)
!36 = !{!37, !38, !39, !40, !41, !42}
!37 = !DIDerivedType(tag: DW_TAG_member, name: "isTxBusy", scope: !35, file: !1, line: 16, baseType: !12, size: 8, align: 8)
!38 = !DIDerivedType(tag: DW_TAG_member, name: "txPtr", scope: !35, file: !1, line: 17, baseType: !11, size: 16, align: 16, offset: 16)
!39 = !DIDerivedType(tag: DW_TAG_member, name: "txBytesRemaining", scope: !35, file: !1, line: 18, baseType: !17, size: 16, align: 16, offset: 32)
!40 = !DIDerivedType(tag: DW_TAG_member, name: "isRxBusy", scope: !35, file: !1, line: 20, baseType: !12, size: 8, align: 8, offset: 48)
!41 = !DIDerivedType(tag: DW_TAG_member, name: "rxPtr", scope: !35, file: !1, line: 21, baseType: !11, size: 16, align: 16, offset: 64)
!42 = !DIDerivedType(tag: DW_TAG_member, name: "rxBytesRemaining", scope: !35, file: !1, line: 22, baseType: !17, size: 16, align: 16, offset: 80)
!43 = !{i32 2, !"Dwarf Version", i32 4}
!44 = !{i32 2, !"Debug Info Version", i32 3}
!45 = !{!"clang version 3.8.0 (http://llvm.org/git/clang.git 52ed5ec631b0bbf5c714baa0cd83c33ebfe0c6aa) (http://llvm.org/git/llvm.git 81386d4b4fdd80f038fd4ebddc59613770ea236c)"}
!46 = !DILocation(line: 35, column: 15, scope: !4)
!47 = !DILocation(line: 36, column: 15, scope: !4)
!48 = !DILocation(line: 69, column: 13, scope: !4)
!49 = !DILocation(line: 70, column: 13, scope: !4)
!50 = !DILocation(line: 71, column: 15, scope: !4)
!51 = !DILocation(line: 76, column: 18, scope: !4)
!52 = !DILocation(line: 77, column: 18, scope: !4)
!53 = !DILocation(line: 79, column: 18, scope: !4)
!54 = !DILocation(line: 80, column: 18, scope: !4)
!55 = !DILocation(line: 91, column: 15, scope: !4)
!56 = !DILocation(line: 94, column: 22, scope: !4)
!57 = !DILocation(line: 95, column: 30, scope: !4)
!58 = !DILocation(line: 96, column: 22, scope: !4)
!59 = !DILocation(line: 97, column: 30, scope: !4)
!60 = !DILocation(line: 99, column: 1, scope: !4)
!61 = !DILocation(line: 106, column: 18, scope: !7)
!62 = !DILocation(line: 107, column: 18, scope: !7)
!63 = !DILocation(line: 108, column: 18, scope: !7)
!64 = !DILocation(line: 109, column: 18, scope: !7)
!65 = !DILocation(line: 110, column: 15, scope: !7)
!66 = !DILocation(line: 111, column: 13, scope: !7)
!67 = !DILocation(line: 112, column: 15, scope: !7)
!68 = !DILocation(line: 113, column: 12, scope: !7)
!69 = !DILocation(line: 114, column: 13, scope: !7)
!70 = !DILocation(line: 115, column: 1, scope: !7)
!71 = !DILocalVariable(name: "txBuf", arg: 1, scope: !8, file: !1, line: 124, type: !11)
!72 = !DIExpression()
!73 = !DILocation(line: 124, column: 30, scope: !8)
!74 = !DILocalVariable(name: "size", arg: 2, scope: !8, file: !1, line: 124, type: !17)
!75 = !DILocation(line: 124, column: 46, scope: !8)
!76 = !DILocation(line: 127, column: 5, scope: !8)
!77 = !DILocation(line: 127, column: 20, scope: !78)
!78 = !DILexicalBlockFile(scope: !79, file: !1, discriminator: 3)
!79 = !DILexicalBlockFile(scope: !8, file: !1, discriminator: 1)
!80 = !DILocation(line: 127, column: 5, scope: !81)
!81 = !DILexicalBlockFile(scope: !8, file: !1, discriminator: 2)
!82 = !DILocation(line: 131, column: 22, scope: !8)
!83 = !DILocation(line: 132, column: 21, scope: !8)
!84 = !DILocation(line: 132, column: 19, scope: !8)
!85 = !DILocation(line: 133, column: 32, scope: !8)
!86 = !DILocation(line: 133, column: 30, scope: !8)
!87 = !DILocation(line: 135, column: 13, scope: !8)
!88 = !DILocation(line: 137, column: 12, scope: !8)
!89 = !DILocation(line: 143, column: 1, scope: !8)
!90 = !DILocalVariable(name: "txBuf", arg: 1, scope: !20, file: !1, line: 152, type: !11)
!91 = !DILocation(line: 152, column: 25, scope: !20)
!92 = !DILocalVariable(name: "size", arg: 2, scope: !20, file: !1, line: 152, type: !17)
!93 = !DILocation(line: 152, column: 41, scope: !20)
!94 = !DILocation(line: 154, column: 20, scope: !20)
!95 = !DILocation(line: 154, column: 27, scope: !20)
!96 = !DILocation(line: 154, column: 5, scope: !20)
!97 = !DILocation(line: 157, column: 5, scope: !20)
!98 = !DILocation(line: 157, column: 20, scope: !99)
!99 = !DILexicalBlockFile(scope: !100, file: !1, discriminator: 3)
!100 = !DILexicalBlockFile(scope: !20, file: !1, discriminator: 1)
!101 = !DILocation(line: 157, column: 5, scope: !102)
!102 = !DILexicalBlockFile(scope: !20, file: !1, discriminator: 2)
!103 = !DILocation(line: 159, column: 1, scope: !20)
!104 = !DILocalVariable(name: "txBuf", arg: 1, scope: !21, file: !1, line: 165, type: !11)
!105 = !DILocation(line: 165, column: 29, scope: !21)
!106 = !DILocalVariable(name: "size", arg: 2, scope: !21, file: !1, line: 165, type: !17)
!107 = !DILocation(line: 165, column: 45, scope: !21)
!108 = !DILocation(line: 168, column: 5, scope: !21)
!109 = !DILocation(line: 168, column: 20, scope: !110)
!110 = !DILexicalBlockFile(scope: !111, file: !1, discriminator: 3)
!111 = !DILexicalBlockFile(scope: !21, file: !1, discriminator: 1)
!112 = !DILocation(line: 168, column: 5, scope: !113)
!113 = !DILexicalBlockFile(scope: !21, file: !1, discriminator: 2)
!114 = !DILocation(line: 172, column: 22, scope: !21)
!115 = !DILocation(line: 173, column: 21, scope: !21)
!116 = !DILocation(line: 173, column: 19, scope: !21)
!117 = !DILocation(line: 174, column: 32, scope: !21)
!118 = !DILocation(line: 174, column: 30, scope: !21)
!119 = !DILocation(line: 176, column: 12, scope: !21)
!120 = !DILocation(line: 178, column: 5, scope: !21)
!121 = !DILocation(line: 178, column: 36, scope: !122)
!122 = !DILexicalBlockFile(scope: !111, file: !1, discriminator: 2)
!123 = !DILocation(line: 179, column: 36, scope: !124)
!124 = distinct !DILexicalBlock(scope: !21, file: !1, line: 178, column: 40)
!125 = !DILocation(line: 179, column: 21, scope: !124)
!126 = !DILocation(line: 179, column: 19, scope: !124)
!127 = !DILocation(line: 180, column: 9, scope: !124)
!128 = !DILocation(line: 180, column: 18, scope: !129)
!129 = !DILexicalBlockFile(scope: !130, file: !1, discriminator: 3)
!130 = !DILexicalBlockFile(scope: !124, file: !1, discriminator: 1)
!131 = !DILocation(line: 180, column: 26, scope: !124)
!132 = !DILocation(line: 180, column: 16, scope: !124)
!133 = !DILocation(line: 180, column: 9, scope: !134)
!134 = !DILexicalBlockFile(scope: !124, file: !1, discriminator: 2)
!135 = !DILocation(line: 182, column: 17, scope: !124)
!136 = !DILocation(line: 185, column: 22, scope: !21)
!137 = !DILocation(line: 186, column: 1, scope: !21)
!138 = !DILocation(line: 192, column: 20, scope: !22)
!139 = !DILocation(line: 192, column: 5, scope: !22)
!140 = !DILocalVariable(name: "rxBuf", arg: 1, scope: !25, file: !1, line: 201, type: !11)
!141 = !DILocation(line: 201, column: 33, scope: !25)
!142 = !DILocalVariable(name: "size", arg: 2, scope: !25, file: !1, line: 201, type: !17)
!143 = !DILocation(line: 201, column: 49, scope: !25)
!144 = !DILocation(line: 204, column: 5, scope: !25)
!145 = !DILocation(line: 204, column: 20, scope: !146)
!146 = !DILexicalBlockFile(scope: !147, file: !1, discriminator: 3)
!147 = !DILexicalBlockFile(scope: !25, file: !1, discriminator: 1)
!148 = !DILocation(line: 204, column: 5, scope: !149)
!149 = !DILexicalBlockFile(scope: !25, file: !1, discriminator: 2)
!150 = !DILocation(line: 208, column: 22, scope: !25)
!151 = !DILocation(line: 209, column: 21, scope: !25)
!152 = !DILocation(line: 209, column: 19, scope: !25)
!153 = !DILocation(line: 210, column: 32, scope: !25)
!154 = !DILocation(line: 210, column: 30, scope: !25)
!155 = !DILocation(line: 212, column: 13, scope: !25)
!156 = !DILocation(line: 214, column: 12, scope: !25)
!157 = !DILocation(line: 219, column: 1, scope: !25)
!158 = !DILocalVariable(name: "rxBuf", arg: 1, scope: !26, file: !1, line: 228, type: !11)
!159 = !DILocation(line: 228, column: 28, scope: !26)
!160 = !DILocalVariable(name: "size", arg: 2, scope: !26, file: !1, line: 228, type: !17)
!161 = !DILocation(line: 228, column: 44, scope: !26)
!162 = !DILocation(line: 230, column: 23, scope: !26)
!163 = !DILocation(line: 230, column: 30, scope: !26)
!164 = !DILocation(line: 230, column: 5, scope: !26)
!165 = !DILocation(line: 233, column: 5, scope: !26)
!166 = !DILocation(line: 233, column: 20, scope: !167)
!167 = !DILexicalBlockFile(scope: !168, file: !1, discriminator: 3)
!168 = !DILexicalBlockFile(scope: !26, file: !1, discriminator: 1)
!169 = !DILocation(line: 233, column: 5, scope: !170)
!170 = !DILexicalBlockFile(scope: !26, file: !1, discriminator: 2)
!171 = !DILocation(line: 235, column: 1, scope: !26)
!172 = !DILocalVariable(name: "rxBuf", arg: 1, scope: !27, file: !1, line: 241, type: !11)
!173 = !DILocation(line: 241, column: 32, scope: !27)
!174 = !DILocalVariable(name: "size", arg: 2, scope: !27, file: !1, line: 241, type: !17)
!175 = !DILocation(line: 241, column: 48, scope: !27)
!176 = !DILocation(line: 244, column: 5, scope: !27)
!177 = !DILocation(line: 244, column: 20, scope: !178)
!178 = !DILexicalBlockFile(scope: !179, file: !1, discriminator: 3)
!179 = !DILexicalBlockFile(scope: !27, file: !1, discriminator: 1)
!180 = !DILocation(line: 244, column: 5, scope: !181)
!181 = !DILexicalBlockFile(scope: !27, file: !1, discriminator: 2)
!182 = !DILocation(line: 248, column: 22, scope: !27)
!183 = !DILocation(line: 249, column: 21, scope: !27)
!184 = !DILocation(line: 249, column: 19, scope: !27)
!185 = !DILocation(line: 250, column: 32, scope: !27)
!186 = !DILocation(line: 250, column: 30, scope: !27)
!187 = !DILocation(line: 252, column: 13, scope: !27)
!188 = !DILocation(line: 254, column: 5, scope: !27)
!189 = !DILocation(line: 254, column: 36, scope: !190)
!190 = !DILexicalBlockFile(scope: !179, file: !1, discriminator: 2)
!191 = !DILocation(line: 255, column: 9, scope: !192)
!192 = distinct !DILexicalBlock(scope: !27, file: !1, line: 254, column: 40)
!193 = !DILocation(line: 255, column: 18, scope: !194)
!194 = !DILexicalBlockFile(scope: !195, file: !1, discriminator: 3)
!195 = !DILexicalBlockFile(scope: !192, file: !1, discriminator: 1)
!196 = !DILocation(line: 255, column: 26, scope: !192)
!197 = !DILocation(line: 255, column: 16, scope: !192)
!198 = !DILocation(line: 255, column: 9, scope: !199)
!199 = !DILexicalBlockFile(scope: !192, file: !1, discriminator: 2)
!200 = !DILocation(line: 257, column: 17, scope: !192)
!201 = !DILocalVariable(name: "rec", scope: !192, file: !1, line: 259, type: !12)
!202 = !DILocation(line: 259, column: 17, scope: !192)
!203 = !DILocation(line: 259, column: 23, scope: !192)
!204 = !DILocation(line: 260, column: 30, scope: !192)
!205 = !DILocation(line: 260, column: 24, scope: !192)
!206 = !DILocation(line: 260, column: 28, scope: !192)
!207 = !DILocation(line: 263, column: 22, scope: !27)
!208 = !DILocation(line: 264, column: 1, scope: !27)
!209 = !DILocation(line: 270, column: 20, scope: !28)
!210 = !DILocation(line: 270, column: 5, scope: !28)
!211 = !DILocation(line: 279, column: 22, scope: !29)
!212 = !DILocation(line: 279, column: 12, scope: !29)
!213 = !DILocation(line: 279, column: 5, scope: !29)
!214 = !DILocalVariable(name: "rec", scope: !30, file: !1, line: 295, type: !12)
!215 = !DILocation(line: 295, column: 13, scope: !30)
!216 = !DILocation(line: 297, column: 13, scope: !30)
!217 = !DILocation(line: 297, column: 5, scope: !30)
!218 = !DILocation(line: 299, column: 9, scope: !219)
!219 = distinct !DILexicalBlock(scope: !30, file: !1, line: 297, column: 60)
!220 = !DILocation(line: 301, column: 37, scope: !221)
!221 = distinct !DILexicalBlock(scope: !219, file: !1, line: 301, column: 13)
!222 = !DILocation(line: 301, column: 13, scope: !219)
!223 = !DILocation(line: 302, column: 19, scope: !224)
!224 = distinct !DILexicalBlock(scope: !221, file: !1, line: 301, column: 41)
!225 = !DILocation(line: 302, column: 17, scope: !224)
!226 = !DILocation(line: 303, column: 34, scope: !224)
!227 = !DILocation(line: 303, column: 28, scope: !224)
!228 = !DILocation(line: 303, column: 32, scope: !224)
!229 = !DILocation(line: 304, column: 9, scope: !224)
!230 = !DILocation(line: 306, column: 26, scope: !231)
!231 = distinct !DILexicalBlock(scope: !219, file: !1, line: 306, column: 13)
!232 = !DILocation(line: 306, column: 15, scope: !231)
!233 = !DILocation(line: 306, column: 13, scope: !219)
!234 = !DILocation(line: 307, column: 20, scope: !235)
!235 = distinct !DILexicalBlock(scope: !231, file: !1, line: 306, column: 44)
!236 = !DILocation(line: 308, column: 30, scope: !235)
!237 = !DILocation(line: 309, column: 9, scope: !235)
!238 = !DILocation(line: 311, column: 9, scope: !219)
!239 = !DILocation(line: 313, column: 36, scope: !219)
!240 = !DILocation(line: 313, column: 21, scope: !219)
!241 = !DILocation(line: 313, column: 19, scope: !219)
!242 = !DILocation(line: 314, column: 13, scope: !243)
!243 = distinct !DILexicalBlock(scope: !219, file: !1, line: 314, column: 13)
!244 = !DILocation(line: 314, column: 40, scope: !243)
!245 = !DILocation(line: 314, column: 13, scope: !219)
!246 = !DILocation(line: 316, column: 13, scope: !247)
!247 = distinct !DILexicalBlock(scope: !243, file: !1, line: 314, column: 46)
!248 = !DILocation(line: 316, column: 20, scope: !249)
!249 = !DILexicalBlockFile(scope: !250, file: !1, discriminator: 3)
!250 = !DILexicalBlockFile(scope: !247, file: !1, discriminator: 1)
!251 = !DILocation(line: 316, column: 30, scope: !247)
!252 = !DILocation(line: 316, column: 13, scope: !253)
!253 = !DILexicalBlockFile(scope: !247, file: !1, discriminator: 2)
!254 = !DILocation(line: 317, column: 20, scope: !247)
!255 = !DILocation(line: 318, column: 30, scope: !247)
!256 = !DILocation(line: 319, column: 9, scope: !247)
!257 = !DILocation(line: 320, column: 9, scope: !219)
!258 = !DILocation(line: 322, column: 9, scope: !219)
!259 = !DILocation(line: 324, column: 9, scope: !219)
!260 = !DILocation(line: 326, column: 1, scope: !30)
