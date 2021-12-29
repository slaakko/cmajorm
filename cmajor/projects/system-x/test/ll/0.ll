; ModuleID = 'test.ll'
source_filename = "test.ll"

; Function Attrs: norecurse nounwind readonly
define i32 @test(i32* nocapture readonly %x, i32* nocapture readnone %y) local_unnamed_addr #0 {
entry:
  %a = load i32, i32* %x, align 4
  ret i32 %a
}

attributes #0 = { norecurse nounwind readonly }
