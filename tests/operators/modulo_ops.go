package main

func main() {
	value := 10
	mod := value % 3
	value %= 4

	mixed := (10 + 5) % 4

	_ = mod
	_ = value
	_ = mixed
}
