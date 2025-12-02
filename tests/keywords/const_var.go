package main

const name string = "demo"
const number int64 = 10
const ratio float64 = 1.5

var ready bool = true
var letter rune = 'A'
var label string = "keyword"

func main() {
	var local string = name
	value := number
	_ = local
	_ = ready
	_ = ratio
	_ = label
	_ = letter
	_ = value
}
