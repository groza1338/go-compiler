package main

func count(values [3]int64) int64 {
	var total int64 = 0
	for range values {
		total = total + 1
	}
	return total
}

func main() {
	var numbers [3]int64
	result := count(numbers)
	_ = result
}
