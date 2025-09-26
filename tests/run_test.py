import os
import glob
import subprocess
from pathlib import Path

DEBUG = False

BASE_RUN_DIRECTORY = "."
BASE_IMAGE_NAME = "golang_flex_project"
TEST_RESULT_DIRECTORY = "result"
EXECUTABLE_TARGET = "./golang_lexer"

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
    command = ["docker", "run", "-d", image_name] + ENDLESS_LOOP_COMMAND

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
        print(f"Container '{image_name}' kill successfully.")
        return result.stdout.replace('\n', '')
    else:
        raise Exception(f"Kill container failed:\n {result.stderr}")


def run_command_inside_container(command: list[str], container_name: str, stdout_file: Path = None):
    # Создание команды
    if stdout_file is not None:
        # Команда с сохранением информации в файл
        run_command = ["(" + "docker", "exec", "-it", container_name] + command + [")", ">>", str(stdout_file)]
    else:
        run_command = ["docker", "exec", "-it", container_name] + command

    # Вывести команду в терминал
    if DEBUG: print(f"Run command inside container {container_name}:\n", " ".join(run_command))

    # Выполнение команды
    result = subprocess.run(" ".join(run_command), text=True, capture_output=True, shell=True)

    # Проверка, успешно ли выполнена команда
    if result.returncode == 0:
        print(f"Exec command {" ".join(command)} successfully.")
    else:
        raise Exception(f"Exec command {" ".join(command)} failed:\n {result.stderr}")


if __name__ == "__main__":
    files = process_golang_files(BASE_RUN_DIRECTORY)
    image_name = build_docker_image()
    container_name = run_docker_container(image_name=image_name)
    try:
        for file in files:
            new_path = Path(TEST_RESULT_DIRECTORY) / Path(file).with_name(Path(file).name).with_suffix('.txt')
            new_path.parent.mkdir(parents=True, exist_ok=True)
            command = [EXECUTABLE_TARGET, Path(file).as_posix()]
            run_command_inside_container(command, container_name, new_path)
    finally:
        kill_docker_container(container_name)
