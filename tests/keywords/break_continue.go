package main

func main() {
	count := 0
	for i := 0; i < 5; i++ {
		if i == 0 {
			continue
		}

		count = count + i

		if count > 5 {
			break
		}
	}

	_ = count
}
