<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Administrator Guide - Group Emails](#administrator-guide---group-emails)
  - [Features](#features)
  - [API Endpoint](#api-endpoint)
    - [Send Group Email](#send-group-email)
  - [Technical Details](#technical-details)
  - [Usage Example (curl)](#usage-example-curl)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Administrator Guide - Group Emails

This document describes how administrators can send group-wide emails with automatic translation.

## Features

- **Group Targeting**: Send messages to all active members of a specific group.
- **Automatic Translation**: The backend automatically translates the message into each recipient's preferred email language using an integrated LLM/Translation API.
- **Authorization**:
    - Global Admins can send emails to any group.
    - Group Admins can send emails to their own group.

## API Endpoint

### Send Group Email

Sends a translated email to all active members of a group.

- **URL**: `/api/admin/group-email`
- **Method**: `POST`
- **Auth required**: YES (Admin or Group Admin)
- **Content-Type**: `application/json`

**Request Body**:
```json
{
  "groupId": "550e8400-e29b-41d4-a716-446655440000",
  "text": "Tomorrow is strawberry cake day in the breakroom!"
}
```

**Response**:
- `200 OK`: `{"message": "Emails are being sent"}`
- `400 Bad Request`: Missing parameters.
- `403 Forbidden`: Not authorized for this group.
- `500 Internal Server Error`: Database or service error.

## Technical Details

1. The backend fetches all active users for the given `groupId`.
2. Users are grouped by their `email_language` preference.
3. For each language, a translation request is sent to the local AI API (`http://localhost:18080/api/v1/prompt`).
4. The translated message is then queued for delivery via SMTP.

## Usage Example (curl)

```bash
curl -X POST http://localhost:8080/api/admin/group-email \
     -H "Authorization: Bearer <YOUR_JWT_TOKEN>" \
     -H "Content-Type: application/json" \
     -d '{
           "groupId": "550e8400-e29b-41d4-a716-446655440000",
           "text": "The kitchen is closed for cleaning until 2 PM."
         }'
```
