package main

func main() {
	arr := [2]int{1, 2}
	for i, j, k := range arr {
		_ = i
		_ = j
	}
}
