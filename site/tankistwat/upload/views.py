from django.http import HttpResponse
from django.shortcuts import render_to_response

def upload(request):
    return render_to_response('upload/index.html')