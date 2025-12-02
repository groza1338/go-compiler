package main

func main() {
	value := 3

	if value > 5 {
		_ = value
	} else {
		_ = value - 1
	}

	if temp := value - 1; temp == 2 {
		_ = temp
	}
}
