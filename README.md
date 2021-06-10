## fterminal

My Psinochka asked me to make a fallout-style terminal with notes function.

I added user accounts, hacking option.
And I wanted to implement an internal scripting language (which [rafael](https://github.com/hellodoge/rafael) was intended to be)

Project is written in pure C, using `ncurses` library

Notes functionality is not implemented yet, but here's what's alredy done:

```
HDG INDUSTRIES(TM) TERMLINK PROTOCOL
ENTER PASSWORD NOW

3 ATTEMPT(S) LEFT:

0xD2C0 {`)||@?:!*~] 0xD38C `}#$#^?/_!`=
0xD2CC %"REGGAE\,]^ 0xD398 "AUTEUR,&_#>
0xD2D8 }&<+#>]}/"{, 0xD3A4 .#-!@|#;{%>}
0xD2E4 [;_[^^:|_,=_ 0xD3B0 })-])(+*(>&'
0xD2F0 |@\?!:GYNURA 0xD3BC $~=)^?`(BEHO
0xD2FC ="@!#<,^"':_ 0xD3C8 OF|"/;~-],%"
0xD308 ~}\_++?(~,TR 0xD3D4 =:+_#:!'/=/)
0xD314 EADS*((`*{{, 0xD3E0 (+:}<[+[~JOT
0xD320 /(%:.?+.ALIE 0xD3EC TER;"}<?.{/: > AUTEUR
0xD32C NE$,(~,#)*=< 0xD3F8 ,/@_"%#)/<LE > Entry denied
0xD338 <^._@*&$;+>} 0xD404 DGER].?;%.!& > 2/6 correct.
0xD344 }|,;}=.+.;@? 0xD410 ,=^]/()^[|>\
0xD350 :VOTIVE_"[?\ 0xD41C "@~:'&:.@&?. > JOTTER
0xD35C ~__;%./#+^=* 0xD428 =)|[SNEEZY^} > Entry denied
0xD368 ,~>];MOTHER< 0xD434 |"\>~,+!!*;( > 4/6 correct.
0xD374 -"-.\!%\_~(} 0xD440 /~?$%NIACIN'
0xD380 -@!\/>!^=><# 0xD44C -*({}!_`#=/" >
```

Rows and columns count, length of each row are configurable in `preferences.h` file.

Besides, shell is fully interactable.

```
WELCOME TO HDG INDUSTRIES(TM) TERMLINK
> USRLIST
Local users:
1. JOHNDOE
2. FOOBAR
> ADDUSR NEWUSER
Enter new password: *****
> USRLIST
Local users:
1. JOHNDOE
2. FOOBAR
3. NEWUSER
```
