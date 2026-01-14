package main

func main() {
	arr := [2]int{1, 2}
	for i, v := range arr {
		_ = i
		_ = v
	}
	for i := range "ab" {
		_ = i
	}
}
