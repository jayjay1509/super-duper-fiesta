import os
import time
import subprocess
import sys
import win32com.client

def is_git_repo(path):
    return os.path.isdir(os.path.join(path, ".git"))

def add_to_startup():
    python_path = sys.executable
    script_path = os.path.abspath(__file__)
    
    task_name = "AutoGitPushScript"

    scheduler = win32com.client.Dispatch('Schedule.Service')
    scheduler.Connect()

    task_definition = scheduler.NewTask(0)
    task_definition.RegistrationInfo.Description = "Execute le script Git auto-push au demarrage."

    task_definition.Principal.RunLevel = 1  # 1 = highest available

    action = task_definition.Actions.Create(0)
    action.Path = python_path
    action.Arguments = f'"{script_path}"'

    trigger = task_definition.Triggers.Create(1)  # 1 = At startup
    trigger.StartBoundary = "2025-01-01T00:00:00"


    rootFolder = scheduler.GetFolder("\\")
    rootFolder.RegisterTaskDefinition(
        task_name,
        task_definition,
        6,  # 6 = Create or update
        None,
        None,
        0,  # 3 = Run with highest privileges
        None
    )

    print(f"Tache creee : {task_name}, elle s'executera au demarrage du systeme.")

def has_changes(path):
    result = subprocess.run(['git', 'status', '--porcelain'], cwd=path, capture_output=True, text=True)
    return len(result.stdout.strip()) > 0

def auto_push(project_path, branch="dev"):
    try:
        current_branch = subprocess.check_output(
            ['git', 'rev-parse', '--abbrev-ref', 'HEAD'], cwd=project_path, text=True).strip()

        # Verifie si la branche 'dev' existe
        branches = subprocess.check_output(['git', 'branch'], cwd=project_path, text=True)
        if f' {branch}' not in branches and f'* {branch}' not in branches:
            print(f"Branche '{branch}' introuvable, creation a partir de '{current_branch}'...")
            subprocess.run(['git', 'checkout', '-b', branch], cwd=project_path, check=True)
            subprocess.run(['git', 'push', '-u', 'origin', branch], cwd=project_path, check=True)
        else:
            subprocess.run(['git', 'checkout', branch], cwd=project_path, check=True)

        if has_changes(project_path):
            print(f"Modifications detectees dans {project_path}, push sur {branch}...")
            subprocess.run(["git", "add", "."], cwd=project_path, check=True)
            subprocess.run(['git', 'commit', '-m', 'Auto commit'], cwd=project_path, check=True)
            subprocess.run(['git', 'push', 'origin', branch], cwd=project_path, check=True)
            print(f"Push termine pour {project_path}")
        else:
            print(f"Aucune modification a pusher pour {project_path}.")

        # Retour a la branche d'origine
        subprocess.run(['git', 'checkout', current_branch], cwd=project_path, check=True)

    except subprocess.CalledProcessError as e:
        print(f"Erreur git dans {project_path}: {e}")

def monitor_project():
    project_path = os.getcwd()
    if is_git_repo(project_path):
        while True:
            auto_push(project_path)
            time.sleep(3600)
    else:
        print(f"Le dossier {project_path} n'est pas un depot Git.")

if __name__ == "__main__":
    if "--once" in sys.argv:
        auto_push(os.getcwd())
    else:
        add_to_startup()
        monitor_project()