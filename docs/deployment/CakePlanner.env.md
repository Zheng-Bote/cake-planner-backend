<!-- DOCTOC SKIP -->
# Example of an .env file for the CakePlanner Backend

```toml
# Server Configuration
CAKE_SERVER_PORT=8080
CAKE_THREADS=4
CAKE_ADMIN_PASSWORD=2Hot4Uh!
CAKE_JWT_SECRET=MySuperSecretLongPasswordForJWTs

# Directories (Relative to Working Directory /app)
DB_DIR=data/cakeplanner.sqlite
LOG_DIR=logs
UPLOAD_DIR=public/uploads

# Logging
LOG_LEVEL=info

# SMTP Configuration (Emails)
SMTP_SERVER=smtp.example.com
SMTP_PORT=587
SMTP_USER=your_email@example.com
SMTP_PASS=your_password
SMTP_FROM=noreply@example.com
SMTP_STARTTLS=true
```

