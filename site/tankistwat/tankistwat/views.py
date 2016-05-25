from django.http import HttpResponse, HttpResponseRedirect
from django.shortcuts import render_to_response
from django.core.mail import send_mail

from tankistwat.forms import ContactForm

import datetime


def hello(request):
    return HttpResponse("Hello world " + str(request.COOKIES.get('auth_token')))

def current_datetime(request):
    now = datetime.datetime.now()
    return render_to_response('current_datetime.html', {'current_date': now})

def main(request):
    return render_to_response('index.html')

def contact(request):
    if request.method == 'POST':
        form = ContactForm(request.POST)
        if form.is_valid():
            cd = form.cleaned_data
            send_mail(
                cd['subject'],
                cd['message'],
                cd.get('e-mail', 'noreply@example.com'),
                ['siteowner@example.com'],
            )
            return HttpResponseRedirect('/contact/tanks/')
        else:
            form = ContactForm()
        return render_to_response('contact_form.html', {'form': form})
    return render_to_response('contact_form.html', {'form': form})