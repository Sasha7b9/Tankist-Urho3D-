from django.contrib import admin
from newsletter.models import Addressee
from newsletter.models import NewsItem

admin.site.register(Addressee)
admin.site.register(NewsItem)

