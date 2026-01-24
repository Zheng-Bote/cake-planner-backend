# Security Overview

This document outlines the security measures implemented in the application to ensure data protection and secure communication.

---

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Security Overview](#security-overview)
  - [Network Security](#network-security)
  - [Web Server Security](#web-server-security)
  - [Authentication \& Data Protection](#authentication--data-protection)
  - [Data Protection](#data-protection)
  - [Email Security](#email-security)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

---

## Network Security

The application enforces **HTTPS (SSL/TLS)** for all connections to the backend. This ensures that all data transmitted between the client and the server is encrypted and protected from eavesdropping.

## Web Server Security

The NGINX web server is configured with robust **Security Headers** to protect against common web vulnerabilities. This includes:

- **Content-Security-Policy (CSP)**: To prevent Cross-Site Scripting (XSS) attacks.
- Other standard security headers to harden the server response.
\
The NGINX web server is configured with **Rate Limiting** to prevent Brute Force or DDoS attacks.

## Authentication & Data Protection

User passwords are encrypted using **Argon2id**, a state-of-the-art password hashing algorithm resilient against GPU-based brute-force attacks.

The backend supports TOTP (Time-based One-Time Password) for an additional layer of login security with **Google Authenticator** or **Microsoft Authenticator**.
\
Users can enable or disable **2FA (Two-Factor Auth entication)** in their profile settings.

## Data Protection

Input validation and routing are using **strong typing** and all database interactions are using **prepared statements** to prevent SQL injection and other common web vulnerabilities.

## Email Security

All email communications sent by the application use a **Mail Gateway** with **Secured Mail (STARTLS)** via an authenticated mail user, ensuring that email traffic is encrypted in transit.
