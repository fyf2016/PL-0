const a=10;
var b,c;
procedure p;
	var d;
    procedure q;
		const e=5;
		var f;
		procedure r;
			var g;
			begin
				g:=a+e;
				f:=g;
				write(g);
			end;
		begin
			call r;
			write(f);
			f:=a+e*f;
			write(f,d);
		end;
	begin
		d:=a+b;
		write(d);
		call q;
	end;
	procedure s;
		begin
			call p;
			b:=-10;
			write(b);
		end;
	begin
		b:=8;
		read(c);
		while c>0 do
			begin
				write(b);
				call s;
				b:=b+c;
				write(b);
				read(c);
			end;
		call p;
	end.