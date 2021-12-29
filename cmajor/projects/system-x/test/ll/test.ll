define i32 @test(i32* %x, i32* %y)
{
entry:
	%a = load i32, i32* %x
	ret i32 %a
foo:
	ret i32 0
exit:
    ret i32 1	
}
