# Shimu Simulator Build & Run Automation
# Architect: Rahat Hasan | Akkhar Labs

# টার্মিনালে বাংলা এনকোডিং সেট করা
$OutputEncoding = [System.Text.Encoding]::UTF8
chcp 65001 | Out-Null

Write-Host "`n--- শিমু হার্ডওয়্যার ইঞ্জিন শুরু হচ্ছে ---" -ForegroundColor Cyan

# ১. কম্পাইল করা (Formatting এবং সব মডিউল সহ)
gcc -Iinclude src/main.c src/cpu.c src/memory.c src/alu.c src/io.c src/utils/formatting.c -o bin/shimu.exe

# চেক করা কম্পাইল সফল হয়েছে কিনা
if ($LASTEXITCODE -eq 0) {
    Write-Host "সংকলন সফল! সিমুলেশন চালানো হচ্ছে...`n" -ForegroundColor Green
    
    # ২. রান করা (test.b ফাইল লোড করা)
    if (Test-Path "test.b") {
        .\bin\shimu.exe test.b
        Write-Host "`nসিমুলেশন রিপোর্ট 'সিমুলেটর_ফলাফল.txt' ফাইলে সংরক্ষিত হয়েছে।" -ForegroundColor Yellow
    }
    else {
        Write-Host "ভুল: 'test.b' ফাইলটি পাওয়া যায়নি!" -ForegroundColor Red
    }
}
else {
    Write-Host "`nভুল: শিমু সংকলন ব্যর্থ হয়েছে!" -ForegroundColor Red
}