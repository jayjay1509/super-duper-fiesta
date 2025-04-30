import os 
import time
import subprocess
import sys
import win32com.client

def is_git_repo(path):
    return os.path.isdir(os.path.join(path, ".git"))

def has_changes(path):
    result = subprocess.run(['git', 'status', '--porcelain'], cwd=path, capture_output=True, text=True)
    return len(result.stdout.strip()) > 0

def auto_push(project_path, branch="dev"):
    try:
        current_branch = subprocess.check_output(
            ['git', 'rev-parse', '--abbrev-ref', 'HEAD'], cwd=project_path, text=True).strip()

        # Stash des changements s'il y en a
        stash_needed = has_changes(project_path)
        if stash_needed:
            subprocess.run(['git', 'stash'], cwd=project_path, check=True)

        # V�rifie si la branche dev existe
        branches = subprocess.check_output(['git', 'branch'], cwd=project_path, text=True)
        if f' {branch}' not in branches and f'* {branch}' not in branches:
            print(f"Branche '{branch}' introuvable, creation � partir de '{current_branch}'...")
            subprocess.run(['git', 'checkout', '-b', branch], cwd=project_path, check=True)
            subprocess.run(['git', 'push', '-u', 'origin', branch], cwd=project_path, check=True)
        else:
            subprocess.run(['git', 'checkout', branch], cwd=project_path, check=True)

        if stash_needed:
            subprocess.run(['git', 'stash', 'pop'], cwd=project_path, check=True)

            print(f"Modifications detectees dans {project_path}, push sur {branch}...")
            subprocess.run(["git", "add", "."], cwd=project_path, check=True)
            subprocess.run(['git', 'commit', '-m', 'Auto commit'], cwd=project_path, check=True)
            subprocess.run(['git', 'push', 'origin', branch], cwd=project_path, check=True)
            print(f"Push termine pour {project_path}")
        else:
            print(f"Aucune modification pusher pour {project_path}.")

        # Retour � la branche d'origine
        subprocess.run(['git', 'checkout', current_branch], cwd=project_path, check=True)

        # Si on �tait revenu avec un stash, remettre les fichiers
        if stash_needed and current_branch != branch:
            subprocess.run(['git', 'stash', 'pop'], cwd=project_path, check=True)

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
        monitor_project() 