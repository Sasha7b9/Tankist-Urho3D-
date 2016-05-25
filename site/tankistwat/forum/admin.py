from django.contrib import admin
from forum.models import Post, Section, Subsectioin, Topic

class PostAdmin(admin.ModelAdmin):
    list_display = ('author', 'publish')
    ist_filter = ('publish',)
    date_hierarchy = 'publish'
    search_fields = ('author', 'publish')

admin.site.register(Post, PostAdmin)
admin.site.register(Section)
admin.site.register(Subsectioin)
admin.site.register(Topic)
