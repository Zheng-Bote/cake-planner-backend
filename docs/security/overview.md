<!-- DOCTOC SKIP -->

# Security Overview

This document outlines the security measures implemented in the application to ensure data protection and secure communication.

## Network Security

The application enforces **HTTPS (SSL/TLS)** for all connections to the backend. This ensures that all data transmitted between the client and the server is encrypted and protected from eavesdropping.

## Web Server Security

The NGINX web server is configured with robust **Security Headers** to protect against common web vulnerabilities. This includes:

- **Content-Security-Policy (CSP)**: To prevent Cross-Site Scripting (XSS) attacks.
- Other standard security headers to harden the server response.

## Authentication & Data Protection

User passwords are encrypted using **Argon2id**, a state-of-the-art password hashing algorithm resilient against GPU-based brute-force attacks.

## Email Security

All email communications sent by the application use **Secured Mail (STARTLS)** via an authenticated mail user, ensuring that email traffic is encrypted in transit.
