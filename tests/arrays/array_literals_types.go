package main

func main() {
	f1 := 1.25
	f2 := 2.5
	r1 := 'Ц'
	r2 := 'Ц'
	s1 := "go"
	s2 := "lang"

	floatArr := [2]float64{f1, f2}
	runeArr := [2]rune{r1, r2}
	stringArr := [2]string{s1, s2}

	_ = floatArr
	_ = runeArr
	_ = stringArr
}
