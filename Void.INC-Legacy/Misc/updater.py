import json,os,requests,zipfile,tempfile,shutil,subprocess,time

class Updater:
    def __init__(self, contributor: str, repo: str):
        self.contributor = contributor
        self.repo = repo
        self.api_url = f"https://api.github.com/repos/{contributor}/{repo}"
        self.headers = {"User-Agent": "Updater"}

    def get_current_version(self):
        try:
            if os.path.exists("V.json"):
                with open("V.json", 'r') as f:
                    return json.load(f).get("V")
            return "1.0.0-release"
        except Exception:
            return None
        
    def get_latest_release(self):
        try:
            response = requests.get(f"{self.api_url}/releases/latest", headers=self.headers)
            response.raise_for_status()
            release = response.json()
            if "assets" in release:
                return release
            return None
        except Exception as e:
            print(f"API Error: {e}")
            return None
        
    def download_and_extract(self, release):
        try:
            zip_asset = next((a for a in release["assets"] if a["name"].endswith(".zip")), None)
            if not zip_asset:
                print("No .zip file found in release assets.")
                return False
            zip_url = zip_asset["browser_download_url"]
            temp_dir = tempfile.mkdtemp()
            zip_path = os.path.join(temp_dir, "update.zip")
            print("Downloading...")
            r = requests.get(zip_url, headers=self.headers)
            with open(zip_path, 'wb') as f:
                f.write(r.content)
            with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                zip_ref.extractall(temp_dir)
            top_level = [os.path.join(temp_dir, d) for d in os.listdir(temp_dir) 
                         if os.path.isdir(os.path.join(temp_dir, d))]
            source_path = top_level[0] if top_level else temp_dir
            return self.stage_files(source_path)
        except Exception as e:
            print(f"Error during download: {e}")
            return None
        
    def stage_files(self, source_dir):
        preserve = {"save.json", "V.json", "Updater_0.1.0.exe"}
        staged_updates = []
        for root, dirs, files in os.walk(source_dir):
            for file in files:
                if file in preserve:
                    continue
                src_file = os.path.join(root, file)
                rel_path = os.path.relpath(src_file, source_dir)
                dest_file = os.path.join(os.getcwd(), rel_path)
                os.makedirs(os.path.dirname(dest_file), exist_ok=True)
                try:
                    shutil.copy2(src_file, dest_file)
                except Exception:
                    temp_new = dest_file + ".new"
                    shutil.copy2(src_file, temp_new)
                    staged_updates.append((temp_new, dest_file))
        return staged_updates
    
    def create_batch_script(self, staged_updates):
        lines = [
            "@echo off",
            "timeout /t 2 /nobreak >nul",
            "taskkill /f /im Void.INC.exe >nul 2>&1",
            "timeout /t 1 /nobreak >nul"
        ]
        for temp_file, dest_file in staged_updates:
            lines.append(f'del /f /q "{dest_file}" >nul 2>&1')
            lines.append(f'move /y "{temp_file}" "{dest_file}" >nul 2>&1')
        lines.append('start "" "Void.INC.exe"')
        lines.append('del "%~f0"')
        with open("finish_update.bat", 'w') as f:
            f.write("\n".join(lines))

    def update(self):
        curr_v = self.get_current_version()
        release = self.get_latest_release()
        if not release: return 1
        latest_v = release["tag_name"]
        if curr_v == latest_v:
            print("Up to date.")
            return 0
        print(f"Updating {curr_v} -> {latest_v}")
        staged = self.download_and_extract(release)
        if staged is not None:
            with open("V.json", 'w') as f:
                json.dump({"V": latest_v}, f, indent=4)
            if staged:
                self.create_batch_script(staged)
                subprocess.Popen(["cmd", "/c", "finish_update.bat"], 
                                 creationflags=subprocess.CREATE_NEW_CONSOLE)
                print("Restarting to apply updates...")
                time.sleep(1)
                os._exit(0)
            print("Update complete.")
            return 0
        return 1

if __name__ == "__main__":
    Updater("artdoesstuff", "Void.INC").update()
