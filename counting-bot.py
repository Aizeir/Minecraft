import discord
from discord.ext import commands

intents = discord.Intents.default()
intents.message_content = True  # nécessaire pour lire les messages

bot = commands.Bot(command_prefix="!", intents=intents)

# Variables de jeu
current_count = 0
last_user_id = None

@bot.event
async def on_ready():
    print(f"Bot connected as {bot.user}")

@bot.event
async def on_message(message):
    global current_count, last_user_id

    # Ne pas répondre aux messages du bot lui-même
    if message.author == bot.user:
        return

    # Vérifie si le message est un nombre entier
    try:
        number = int(message.content.strip())
    except ValueError:
        return  # Ignorer les messages non numériques

    # Vérifie si l'utilisateur est le même que le dernier
    if message.author.id == last_user_id:
        await message.channel.send(f"{message.author.mention} Tu ne peux pas jouer deux fois de suite !")
        return

    # Vérifie si le nombre est bien le suivant
    if number == current_count + 1:
        current_count += 1
        last_user_id = message.author.id
        await message.add_reaction("✅")
    else:
        await message.channel.send(f"{message.author.mention} Mauvais nombre. On attendait **{current_count + 1}**.")

# Lancer le bot
bot.run("https://discord.com/api/webhooks/1378628024332976139/DIv9Z-XcDFsUUZVZHxb2kC-we0KEO-2ZBzipfZ1xvSNP0pK2oLMOnTOo7r041jiUuNJg")
