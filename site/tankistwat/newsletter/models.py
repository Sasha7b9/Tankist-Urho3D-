from django.db import models
from django.utils import timezone

class Addressee(models.Model):
    email = models.EmailField()

    def __str__(self):
        return self.email

class NewsItem(models.Model):
    news = models.TextField()
    publish = models.DateTimeField(default=timezone.now)

    def __str__(self):
        return sellf.news;
