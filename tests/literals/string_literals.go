package main

var interpreted = "Line1\nLine2\tTabbed \"quote\""
var unicodeText = "Snowman: \u2603"
var raw = `Raw string keeps \n literal
and spans two lines.`

func main() {
	_ = interpreted + unicodeText
	_ = raw
}
