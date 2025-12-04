package main

func main() {
	sum := 0
	var values [3]int
	values[0] = 1
	values[1] = 2
	values[2] = 3

	for _, v := range values {
		sum = sum + v
	}
	_ = sum
}
