program procTest (input, output);
 var a, b : integer;
var     x : array [1..5] of real;
  procedure one (i, j : integer; k : array [1..5] of real);
    var n : integer;
    begin
      n := i + j;
      k[n] := 2.345
    end
  begin
    a := 1;
    b := 2;
    one(a,b,x);
    write(x)
  end.


