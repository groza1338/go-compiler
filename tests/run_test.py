import os
import glob
import shutil
import subprocess
from pathlib import Path, PurePosixPath

DEBUG = True

BASE_RUN_DIRECTORY = "."
BASE_IMAGE_NAME = "golang_flex_project"
TEST_RESULT_DIRECTORY = "result"
EXECUTABLE_TARGET = "./golang_compiler"
LOCAL_RESULT_DIRECTORY = "./result"
DOCKER_RESULT_DIRECTORY = "/app/result"
DIRECTORY_BINDINGS = LOCAL_RESULT_DIRECTORY + ":" + DOCKER_RESULT_DIRECTORY

ENDLESS_LOOP_COMMAND = ["tail", "-f", "/dev/null"]


def process_golang_files(directory):
    # Формируем путь с расширением .go
    golang_files_pattern = os.path.join(directory, '**', '*.go')

    # Ищем все файлы, которые соответствуют шаблону
    golang_files = glob.glob(golang_files_pattern, recursive=True)

    # Обрабатываем каждый найденный файл
    return golang_files


def build_docker_image(dockerfile_path=".", image_name=BASE_IMAGE_NAME):
    # Команда для сборки Docker-образа
    command = ["docker", "build", "-t", image_name, dockerfile_path]

    # Вывести команду в терминал
    if DEBUG: print("Build image command:\n", " ".join(command))

    # Выполнение команды
    result = subprocess.run(command, text=True, capture_output=True)

    # Проверка, успешно ли выполнена команда
    if result.returncode == 0:
        print(f"Image '{image_name}' built successfully.")
        return image_name
    else:
        raise Exception(f"Build image failed:\n {result.stderr}")


def run_docker_container(image_name=BASE_IMAGE_NAME):
    # Команда для запуска docker контейнера в фоновом режиме
    command = ["docker", "run", "-v", DIRECTORY_BINDINGS, "-d", image_name] + ENDLESS_LOOP_COMMAND

    if DEBUG: print("Run container command:\n", " ".join(command))

    # Выполнение команды
    result = subprocess.run(command, text=True, capture_output=True)

    # Проверка, успешно ли выполнена команда
    if result.returncode == 0:
        print(f"Container '{image_name}' run successfully.")
        return result.stdout.replace('\n', '')
    else:
        raise Exception(f"Run container failed:\n {result.stderr}")


def kill_docker_container(container_name):
    # Команда для завершения работы контейнера
    command = ["docker", "kill", container_name]

    if DEBUG: print("Kill container command:\n", " ".join(command))

    # Выполнение команды
    result = subprocess.run(command, text=True, capture_output=True)

    # Проверка, успешно ли выполнена команда
    if result.returncode == 0:
        print(f"Container '{container_name}' kill successfully.")
        return result.stdout.replace('\n', '')
    else:
        raise Exception(f"Kill container failed:\n {result.stderr}")


def run_command_inside_container(command: list[str], container_name: str):
    exec_prefix = ["docker", "exec", "-i", container_name]
    run_command = exec_prefix + command

    if DEBUG: print(f"Run command inside container {container_name}:\n", " ".join(run_command))

    result = subprocess.run(
        run_command,
        text=True,
        capture_output=True,
        encoding="utf-8",
        errors="replace",
    )

    if result.returncode == 0:
        print(f"SUCCESS: Exec command {" ".join(command)} successfully.")
    else:
        print(f"FAILED: Exec command {" ".join(command)} failed with code {result.returncode}:\n {result.stderr}")

    return result


def clear_test_results_dir(directory: str):
    results_dir = os.path.join(directory, TEST_RESULT_DIRECTORY)

    if os.path.exists(results_dir) and os.path.isdir(results_dir):
        shutil.rmtree(results_dir)
        print(f"Directory {TEST_RESULT_DIRECTORY} and its contents have been removed.")
    else:
        print(f"Directory {TEST_RESULT_DIRECTORY} does not exist in {directory}.")

if __name__ == "__main__":
    files = process_golang_files(BASE_RUN_DIRECTORY)
    clear_test_results_dir(BASE_RUN_DIRECTORY)
    Path(TEST_RESULT_DIRECTORY).mkdir(parents=True, exist_ok=True)
    image_name = build_docker_image()
    container_name = run_docker_container(image_name=image_name)
    try:
        for file in files:
            file_path = Path(file)
            test_dir = (Path(TEST_RESULT_DIRECTORY) / file_path).with_suffix("")
            test_dir.mkdir(parents=True, exist_ok=True)

            base_name = file_path.stem
            txt_output = test_dir / f"{base_name}.txt"
            dot_output = test_dir / f"{base_name}.dot"
            png_output = test_dir / f"{base_name}.png"

            command = [EXECUTABLE_TARGET, file_path.as_posix()]
            result = run_command_inside_container(command, container_name)

            safe_stdout = result.stdout if result.stdout is not None else ""
            txt_output.write_text(safe_stdout, encoding="utf-8", errors="replace")

            digraph_start = safe_stdout.find("digraph AST")
            if digraph_start != -1:
                dot_content = safe_stdout[digraph_start:]
            else:
                dot_content = 'digraph AST {\n  info[label="No AST produced"];\n}\n'
                if DEBUG:
                    print(f"No AST produced for {file_path}, writing placeholder graph.")

            dot_output.write_text(dot_content, encoding="utf-8", errors="replace")

            relative_dot = dot_output.relative_to(TEST_RESULT_DIRECTORY).as_posix()
            relative_png = png_output.relative_to(TEST_RESULT_DIRECTORY).as_posix()
            container_dot_path = PurePosixPath(DOCKER_RESULT_DIRECTORY) / relative_dot
            container_png_path = PurePosixPath(DOCKER_RESULT_DIRECTORY) / relative_png

            dot_result = run_command_inside_container(
                ["dot", "-Tpng", container_dot_path.as_posix(), "-o", container_png_path.as_posix()],
                container_name
            )
            if dot_result.returncode != 0 and DEBUG:
                print(f"Failed to render PNG for {file_path}: {dot_result.stderr}")
    finally:
        kill_docker_container(container_name)
