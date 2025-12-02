package main

func pick(value int64) int64 {
	switch value {
	case 0:
		return value
	case 1:
		return value + 1
	default:
		return value + 2
	}
}

func main() {
	result := pick(1)
	_ = result
}
