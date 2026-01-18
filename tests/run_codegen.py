import os
import glob
import shutil
import subprocess
from pathlib import Path, PurePosixPath

DEBUG = True

BASE_RUN_DIRECTORY = "code_examples"
BASE_IMAGE_NAME = "golang_flex_project"
EXECUTABLE_TARGET = "./golang_compiler"
LOCAL_CLASSES_DIRECTORY = "./generated_classes"
DOCKER_CLASSES_DIRECTORY = "/app/generated_classes"

ENDLESS_LOOP_COMMAND = ["tail", "-f", "/dev/null"]


def process_golang_files(directory):
    golang_files_pattern = os.path.join(directory, '**', '*.go')
    return glob.glob(golang_files_pattern, recursive=True)


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
        return result.stdout.replace('\n', '')
    raise Exception(f"Run container failed:\n {result.stderr}")


def kill_docker_container(container_name):
    command = ["docker", "kill", container_name]

    if DEBUG:
        print("Kill container command:\n", " ".join(command))

    result = subprocess.run(command, text=True, capture_output=True)
    if result.returncode == 0:
        print(f"Container '{container_name}' kill successfully.")
        return result.stdout.replace('\n', '')
    raise Exception(f"Kill container failed:\n {result.stderr}")


def run_command_inside_container(command: list[str], container_name: str, extra_env: dict | None = None):
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
        print(f"SUCCESS: Exec command {" ".join(command)} successfully.")
    else:
        print(f"FAILED: Exec command {" ".join(command)} failed with code {result.returncode}:\n {result.stderr}")

    return result


def clear_generated_classes_dir(directory: str):
    classes_dir = os.path.join(directory, "generated_classes")

    if os.path.exists(classes_dir) and os.path.isdir(classes_dir):
        shutil.rmtree(classes_dir)
        print("Directory generated_classes and its contents have been removed.")
    else:
        print("Directory generated_classes does not exist.")


if __name__ == "__main__":
    files = process_golang_files(BASE_RUN_DIRECTORY)
    clear_generated_classes_dir(".")
    Path(LOCAL_CLASSES_DIRECTORY).mkdir(parents=True, exist_ok=True)
    image_name = build_docker_image()
    container_name = run_docker_container(image_name=image_name)
    try:
        for file in files:
            file_path = Path(file)
            command = [EXECUTABLE_TARGET, file_path.as_posix()]
            result = run_command_inside_container(command, container_name)
            if result.returncode != 0:
                continue
            if DEBUG and result.stdout:
                print(result.stdout)

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
    finally:
        kill_docker_container(container_name)
