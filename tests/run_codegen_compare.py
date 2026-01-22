import glob
import os
import shutil
import subprocess
from pathlib import Path

DEBUG = True

BASE_RUN_DIRECTORY = "main_code_examples"
BASE_IMAGE_NAME = "golang_flex_project"
EXECUTABLE_TARGET = "./golang_compiler"
LOCAL_CLASSES_DIRECTORY = "./generated_classes"
DOCKER_CLASSES_DIRECTORY = "/app/generated_classes"
LOCAL_SEM_DIRECTORY = os.path.join(LOCAL_CLASSES_DIRECTORY, "sem")
DOCKER_SEM_DIRECTORY = "/app/generated_classes/sem"

ENDLESS_LOOP_COMMAND = ["tail", "-f", "/dev/null"]


def process_golang_files(directory):
    golang_files_pattern = os.path.join(directory, "**", "*.go")
    return glob.glob(golang_files_pattern, recursive=True)


def select_golang_file(files):
    if not files:
        return []
    files = sorted(files)
    print("Выберите файл для компиляции:")
    print("-1) Все файлы")
    for idx, file_path in enumerate(files, start=1):
        print(f"{idx}) {file_path}")
    while True:
        choice = input("Номер файла (или Enter для отмены): ").strip()
        if choice == "":
            return []
        if choice == "-1":
            return files
        if choice.isdigit():
            number = int(choice)
            if 1 <= number <= len(files):
                return [files[number - 1]]
        print("Некорректный выбор, попробуйте снова.")


def build_docker_image(dockerfile_path=".", image_name=BASE_IMAGE_NAME):
    command = ["docker", "build", "-t", image_name, dockerfile_path]

    if DEBUG:
        print("Build image command:\n", " ".join(command))

    result = subprocess.run(command, text=True, capture_output=True)
    if result.returncode == 0:
        print(f"Image '{image_name}' built successfully.")
        return image_name
    raise Exception(f"Build image failed:\n {result.stderr}")


def run_docker_container(image_name=BASE_IMAGE_NAME):
    host_classes_dir = str(Path(LOCAL_CLASSES_DIRECTORY).resolve())
    command = [
        "docker", "run",
        "-v", host_classes_dir + ":" + DOCKER_CLASSES_DIRECTORY,
        "-d", image_name
    ] + ENDLESS_LOOP_COMMAND

    if DEBUG:
        print("Run container command:\n", " ".join(command))

    result = subprocess.run(command, text=True, capture_output=True)
    if result.returncode == 0:
        print(f"Container '{image_name}' run successfully.")
        return result.stdout.replace("\n", "")
    raise Exception(f"Run container failed:\n {result.stderr}")


def kill_docker_container(container_name):
    command = ["docker", "kill", container_name]

    if DEBUG:
        print("Kill container command:\n", " ".join(command))

    result = subprocess.run(command, text=True, capture_output=True)
    if result.returncode == 0:
        print(f"Container '{container_name}' kill successfully.")
        return result.stdout.replace("\n", "")
    raise Exception(f"Kill container failed:\n {result.stderr}")


def run_command_inside_container(command, container_name, extra_env=None):
    exec_prefix = ["docker", "exec", "-i"]
    if extra_env:
        for key, value in extra_env.items():
            exec_prefix.extend(["-e", f"{key}={value}"])
    exec_prefix.append(container_name)
    run_command = exec_prefix + command

    if DEBUG:
        print(f"Run command inside container {container_name}:\n", " ".join(run_command))

    result = subprocess.run(
        run_command,
        text=True,
        capture_output=True,
        encoding="utf-8",
        errors="replace",
    )

    if result.returncode == 0:
        print(f"SUCCESS: Exec command {' '.join(command)} successfully.")
    else:
        print(f"FAILED: Exec command {' '.join(command)} failed with code {result.returncode}:\n {result.stderr}")
        if result.stdout:
            print(result.stdout)

    return result


def clear_generated_classes_dir(directory):
    classes_dir = os.path.join(directory, "generated_classes")

    if os.path.exists(classes_dir) and os.path.isdir(classes_dir):
        shutil.rmtree(classes_dir)
        print("Directory generated_classes and its contents have been removed.")
    else:
        print("Directory generated_classes does not exist.")


def run_local_go(file_path, stdin_text, timeout_seconds=10):
    command = ["go", "run", file_path]
    try:
        return subprocess.run(
            command,
            input=stdin_text,
            text=True,
            capture_output=True,
            encoding="utf-8",
            errors="replace",
            timeout=timeout_seconds,
        )
    except subprocess.TimeoutExpired:
        return None


def run_local_java(class_dir, stdin_text, timeout_seconds=10):
    command = ["java", "-cp", class_dir, "Main"]
    try:
        return subprocess.run(
            command,
            input=stdin_text,
            text=True,
            capture_output=True,
            encoding="utf-8",
            errors="replace",
            timeout=timeout_seconds,
        )
    except subprocess.TimeoutExpired:
        return None


def normalize_output(text):
    return text.rstrip()


def print_stream(label, text):
    print(f"{label}:")
    if text == "":
        print("<empty>")
        return
    end = "" if text.endswith("\n") else "\n"
    print(text, end=end)


def read_test_input(file_path):
    input_path = file_path.with_suffix(file_path.suffix + ".input")
    if input_path.exists():
        return input_path.read_text(encoding="utf-8", errors="replace")
    return None


def prompt_test_input(file_path):
    print(f"Введите stdin для {file_path} (пустая строка завершает ввод):")
    lines = []
    while True:
        try:
            line = input()
        except EOFError:
            break
        if line == "":
            break
        lines.append(line)
    if not lines:
        return ""
    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    files = process_golang_files(BASE_RUN_DIRECTORY)
    selected_files = select_golang_file(files)
    if not selected_files:
        print("Файл не выбран.")
        raise SystemExit(0)
    clear_generated_classes_dir(".")
    Path(LOCAL_CLASSES_DIRECTORY).mkdir(parents=True, exist_ok=True)
    Path(LOCAL_SEM_DIRECTORY).mkdir(parents=True, exist_ok=True)
    image_name = build_docker_image()
    container_name = run_docker_container(image_name=image_name)
    try:
        for file in selected_files:
            file_path = Path(file)
            stdin_text = read_test_input(file_path)
            if stdin_text is None:
                stdin_text = prompt_test_input(file_path)
            go_result = run_local_go(file_path.as_posix(), stdin_text)
            if go_result is None:
                print(f"[{file}] go run timed out.")
                continue
            if go_result.returncode != 0:
                print(f"[{file}] go run failed:\n{go_result.stderr}")
                continue
            command = [EXECUTABLE_TARGET, file_path.as_posix()]
            result = run_command_inside_container(
                command,
                container_name,
                extra_env={"SEMANTIC_OUT_DIR": DOCKER_SEM_DIRECTORY}
            )
            if result.returncode != 0:
                print(f"[{file}] compilation failed.")
                continue
            sem_file = Path(LOCAL_SEM_DIRECTORY) / (file_path.name + ".sem.txt")
            if sem_file.exists():
                print(sem_file.read_text(encoding="utf-8", errors="replace"))

            generated = Path(LOCAL_CLASSES_DIRECTORY) / "Main.class"
            if not generated.exists():
                if DEBUG:
                    print(f"No class produced for {file_path}")
                continue

            try:
                rel_path = file_path.relative_to(BASE_RUN_DIRECTORY).with_suffix("")
            except ValueError:
                rel_path = file_path.with_suffix("")
            dest_dir = Path(LOCAL_CLASSES_DIRECTORY) / rel_path
            dest_dir.mkdir(parents=True, exist_ok=True)
            shutil.copy2(generated, dest_dir / "Main.class")
            generated.unlink(missing_ok=True)

            java_result = run_local_java(str(dest_dir), stdin_text)
            if java_result is None:
                print(f"[{file}] java run timed out.")
                continue
            if java_result.returncode != 0:
                print(f"[{file}] java run failed:\n{java_result.stderr}")
                continue

            go_output = normalize_output(go_result.stdout)
            java_output = normalize_output(java_result.stdout)
            match = go_output == java_output
            print(f"== {file} ==")
            print_stream("stdin", stdin_text)
            print_stream("go output", go_result.stdout)
            print_stream("java output", java_result.stdout)
            print("match:", "yes" if match else "no")
    finally:
        kill_docker_container(container_name)
